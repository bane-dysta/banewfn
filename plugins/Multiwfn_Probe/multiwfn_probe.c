// gcc -O2 -Wall -Wextra -std=c11 -o multiwfn_probe multiwfn_probe.c -lutil -static

#define _XOPEN_SOURCE 700
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pty.h>
#include <signal.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_BUF (32 * 1024 * 1024)
#define READ_CHUNK 4096
#define MAX_PATH_DEPTH 64
#define MAX_TOKEN_LEN 512
#define DEFAULT_BOOT_TIMEOUT_MS 20000
#define DEFAULT_STEP_TIMEOUT_MS 30000
#define DEFAULT_QUIET_MS 600
#define DEFAULT_MAX_NODES 512
#define DEFAULT_MENU_SCAN_LINES 6
#define PATH_SEP '|'

typedef enum {
    NODE_UNKNOWN = 0,
    NODE_MENU,
    NODE_PROMPT,
    NODE_TEXT,
    NODE_TIMEOUT,
    NODE_EOF,
    NODE_ERROR
} NodeState;

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} Buffer;

typedef struct {
    char *token;
    char *label;
} OptionItem;

typedef struct {
    char *path;
    char *parent_path;
    int depth;
    char *input_sent;
    NodeState state;
    char *menu_title;
    char *prompt_line;
    OptionItem *options;
    int nopt;
    char *raw_file;
    char *text_file;
    char *run_dir;
    char **created_files;
    int n_created;
    char **modified_files;
    int n_modified;
    int order;
} NodeRecord;

typedef struct {
    char name[512];
    off_t size;
    time_t mtime;
} FileInfo;

typedef struct {
    FileInfo *items;
    int count;
    int cap;
} DirSnapshot;

typedef struct {
    char token[MAX_TOKEN_LEN];
} PathToken;

typedef struct {
    PathToken items[MAX_PATH_DEPTH];
    int depth;
} PathSpec;

typedef struct {
    PathSpec path;
    int rel_depth;
} QueueItem;

typedef struct {
    NodeRecord *items;
    int count;
    int cap;
} NodeStore;

typedef struct {
    char *bin_path;
    char *input_path;
    char *out_dir;
    PathSpec start_path;
    int expand_depth;
    int boot_timeout_ms;
    int step_timeout_ms;
    int quiet_ms;
    int max_nodes;
} Config;

static void die(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    exit(1);
}

static void *xmalloc(size_t n) {
    void *p = malloc(n ? n : 1);
    if (!p) die("malloc failed");
    return p;
}

static void *xrealloc(void *p, size_t n) {
    void *q = realloc(p, n ? n : 1);
    if (!q) die("realloc failed");
    return q;
}

static char *xstrdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s);
    char *p = (char *)xmalloc(n + 1);
    memcpy(p, s, n + 1);
    return p;
}

static void buf_init(Buffer *b) {
    b->cap = 16384;
    b->len = 0;
    b->data = (char *)xmalloc(b->cap);
    b->data[0] = '\0';
}

static void buf_free(Buffer *b) {
    free(b->data);
    b->data = NULL;
    b->len = b->cap = 0;
}

static void buf_append(Buffer *b, const char *s, size_t n) {
    if (b->len + n + 1 > MAX_BUF) {
        size_t drop = (b->len + n + 1) - MAX_BUF;
        if (drop > b->len) drop = b->len;
        memmove(b->data, b->data + drop, b->len - drop);
        b->len -= drop;
        b->data[b->len] = '\0';
    }
    if (b->len + n + 1 > b->cap) {
        while (b->len + n + 1 > b->cap) b->cap *= 2;
        b->data = (char *)xrealloc(b->data, b->cap);
    }
    memcpy(b->data + b->len, s, n);
    b->len += n;
    b->data[b->len] = '\0';
}

static bool contains_nocase(const char *hay, const char *needle) {
    if (!hay || !needle || !*needle) return true;
    size_t n = strlen(needle);
    for (const char *p = hay; *p; ++p) {
        size_t i = 0;
        while (i < n && p[i] && tolower((unsigned char)p[i]) == tolower((unsigned char)needle[i])) i++;
        if (i == n) return true;
    }
    return false;
}

static char *trimdup_n(const char *s, size_t n) {
    while (n > 0 && isspace((unsigned char)*s)) {
        s++;
        n--;
    }
    while (n > 0 && isspace((unsigned char)s[n - 1])) n--;
    char *p = (char *)xmalloc(n + 1);
    memcpy(p, s, n);
    p[n] = '\0';
    return p;
}

static const char *state_name(NodeState st) {
    switch (st) {
        case NODE_MENU: return "menu";
        case NODE_PROMPT: return "prompt";
        case NODE_TEXT: return "text";
        case NODE_TIMEOUT: return "timeout";
        case NODE_EOF: return "eof";
        case NODE_ERROR: return "error";
        default: return "unknown";
    }
}

static int ensure_dir(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) return 0;
        errno = ENOTDIR;
        return -1;
    }
    return mkdir(path, 0775);
}

static void mkdir_p(const char *path) {
    char *tmp = xstrdup(path);
    size_t n = strlen(tmp);
    if (n == 0) {
        free(tmp);
        return;
    }
    for (size_t i = 1; i < n; ++i) {
        if (tmp[i] == '/') {
            tmp[i] = '\0';
            if (tmp[0] && ensure_dir(tmp) < 0 && errno != EEXIST) die("mkdir failed: %s", tmp);
            tmp[i] = '/';
        }
    }
    if (ensure_dir(tmp) < 0 && errno != EEXIST) die("mkdir failed: %s", tmp);
    free(tmp);
}

static char *path_dirname(const char *path) {
    const char *slash = strrchr(path, '/');
    if (!slash) return xstrdup(".");
    if (slash == path) return xstrdup("/");
    return trimdup_n(path, (size_t)(slash - path));
}

static char *resolve_path(const char *path) {
    char *resolved = realpath(path, NULL);
    if (resolved) return resolved;
    return xstrdup(path);
}

static void write_text_file(const char *path, const char *text) {
    FILE *fp = fopen(path, "wb");
    if (!fp) die("cannot open for write: %s", path);
    if (text && *text) fwrite(text, 1, strlen(text), fp);
    fclose(fp);
}

static void append_json_string(FILE *fp, const char *s) {
    fputc('"', fp);
    if (s) {
        for (const unsigned char *p = (const unsigned char *)s; *p; ++p) {
            switch (*p) {
                case '\\': fputs("\\\\", fp); break;
                case '"': fputs("\\\"", fp); break;
                case '\b': fputs("\\b", fp); break;
                case '\f': fputs("\\f", fp); break;
                case '\n': fputs("\\n", fp); break;
                case '\r': fputs("\\r", fp); break;
                case '\t': fputs("\\t", fp); break;
                default:
                    if (*p < 0x20) fprintf(fp, "\\u%04x", *p);
                    else fputc(*p, fp);
            }
        }
    }
    fputc('"', fp);
}

static bool pathspec_equal(const PathSpec *a, const PathSpec *b) {
    if (a->depth != b->depth) return false;
    for (int i = 0; i < a->depth; ++i) {
        if (strcmp(a->items[i].token, b->items[i].token) != 0) return false;
    }
    return true;
}

static void pathspec_copy(PathSpec *dst, const PathSpec *src) {
    dst->depth = src->depth;
    for (int i = 0; i < src->depth; ++i) {
        strncpy(dst->items[i].token, src->items[i].token, MAX_TOKEN_LEN - 1);
        dst->items[i].token[MAX_TOKEN_LEN - 1] = '\0';
    }
}

static char *pathspec_join(const PathSpec *p) {
    if (p->depth == 0) return xstrdup("");
    size_t total = 1;
    for (int i = 0; i < p->depth; ++i) total += strlen(p->items[i].token) + 1;
    char *out = (char *)xmalloc(total);
    out[0] = '\0';
    for (int i = 0; i < p->depth; ++i) {
        if (i) strcat(out, "|");
        strcat(out, p->items[i].token);
    }
    return out;
}

static char *pathspec_parent_string(const PathSpec *p) {
    if (p->depth == 0) return NULL;
    PathSpec tmp;
    pathspec_copy(&tmp, p);
    tmp.depth--;
    return pathspec_join(&tmp);
}

static void pathspec_push(PathSpec *p, const char *token) {
    if (p->depth >= MAX_PATH_DEPTH) die("path depth exceeds %d", MAX_PATH_DEPTH);
    strncpy(p->items[p->depth].token, token, MAX_TOKEN_LEN - 1);
    p->items[p->depth].token[MAX_TOKEN_LEN - 1] = '\0';
    p->depth++;
}

static void parse_pathspec(PathSpec *out, const char *text) {
    out->depth = 0;
    if (!text || !*text) return;
    const char *p = text;
    while (*p) {
        const char *q = strchr(p, PATH_SEP);
        size_t n = q ? (size_t)(q - p) : strlen(p);
        char *tok = trimdup_n(p, n);
        if (*tok) pathspec_push(out, tok);
        free(tok);
        if (!q) break;
        p = q + 1;
    }
}

static char *sanitize_for_filename(const char *s) {
    if (!s || !*s) return xstrdup("root");
    size_t n = strlen(s);
    char *out = (char *)xmalloc(n * 3 + 1);
    size_t j = 0;
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)s[i];
        if (isalnum(c)) out[j++] = (char)c;
        else if (c == '-') { out[j++] = 'm'; }
        else if (c == '|') { out[j++] = '_'; out[j++] = '_'; }
        else if (c == '_') { out[j++] = '_'; }
        else if (c == ' ') { out[j++] = '_'; }
        else { out[j++] = '_'; }
    }
    out[j] = '\0';
    if (j == 0) strcpy(out, "root");
    return out;
}

static void dirsnap_init(DirSnapshot *snap) {
    snap->items = NULL;
    snap->count = 0;
    snap->cap = 0;
}

static void dirsnap_free(DirSnapshot *snap) {
    free(snap->items);
    snap->items = NULL;
    snap->count = snap->cap = 0;
}

static void dirsnap_add(DirSnapshot *snap, const FileInfo *fi) {
    if (snap->count == snap->cap) {
        snap->cap = snap->cap ? snap->cap * 2 : 16;
        snap->items = (FileInfo *)xrealloc(snap->items, snap->cap * sizeof(FileInfo));
    }
    snap->items[snap->count++] = *fi;
}

static int cmp_fileinfo(const void *a, const void *b) {
    const FileInfo *fa = (const FileInfo *)a;
    const FileInfo *fb = (const FileInfo *)b;
    return strcmp(fa->name, fb->name);
}

static void capture_dirsnapshot(const char *dir, DirSnapshot *snap) {
    DIR *dp = opendir(dir);
    if (!dp) return;
    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;
        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", dir, de->d_name);
        struct stat st;
        if (stat(path, &st) != 0) continue;
        if (!S_ISREG(st.st_mode)) continue;
        FileInfo fi;
        memset(&fi, 0, sizeof(fi));
        snprintf(fi.name, sizeof(fi.name), "%s", de->d_name);
        fi.size = st.st_size;
        fi.mtime = st.st_mtime;
        dirsnap_add(snap, &fi);
    }
    closedir(dp);
    qsort(snap->items, snap->count, sizeof(FileInfo), cmp_fileinfo);
}

static const FileInfo *dirsnap_find(const DirSnapshot *snap, const char *name) {
    int lo = 0, hi = snap->count - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        int cmp = strcmp(name, snap->items[mid].name);
        if (cmp == 0) return &snap->items[mid];
        if (cmp < 0) hi = mid - 1;
        else lo = mid + 1;
    }
    return NULL;
}

static void add_string(char ***arr, int *count, const char *s) {
    *arr = (char **)xrealloc(*arr, (size_t)(*count + 1) * sizeof(char *));
    (*arr)[*count] = xstrdup(s);
    (*count)++;
}

static void diff_dirs(const DirSnapshot *before, const DirSnapshot *after,
                      char ***created, int *n_created,
                      char ***modified, int *n_modified) {
    *created = NULL; *modified = NULL; *n_created = 0; *n_modified = 0;
    for (int i = 0; i < after->count; ++i) {
        const FileInfo *a = &after->items[i];
        if (strcmp(a->name, "settings.ini") == 0) continue;
        const FileInfo *b = dirsnap_find(before, a->name);
        if (!b) add_string(created, n_created, a->name);
        else if (b->size != a->size || b->mtime != a->mtime) add_string(modified, n_modified, a->name);
    }
}

static int send_line(int fd, const char *line) {
    size_t len = strlen(line);
    if (write(fd, line, len) < 0) return -1;
    if (write(fd, "\n", 1) < 0) return -1;
    return 0;
}

static bool output_has_file_prompt(const char *text) {
    return contains_nocase(text, "Now input file path");
}

static char *normalize_tty_text(const char *raw) {
    size_t n = strlen(raw);
    char *out = (char *)xmalloc(n + 1);
    size_t olen = 0;
    size_t line_start = 0;
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)raw[i];
        if (c == '\r') {
            if (i + 1 < n && raw[i + 1] == '\n') {
                out[olen++] = '\n';
                line_start = olen;
                i++;
            } else {
                olen = line_start;
            }
            continue;
        }
        if (c == '\n') {
            out[olen++] = '\n';
            line_start = olen;
            continue;
        }
        if (c == '\t' || (c >= 0x20 && c != 0x7f)) {
            out[olen++] = (char)c;
        }
    }
    out[olen] = '\0';
    return out;
}

static pid_t spawn_under_pty(const char *prog, const char *workdir, int *master_fd) {
    pid_t pid = forkpty(master_fd, NULL, NULL, NULL);
    if (pid < 0) die("forkpty failed");
    if (pid == 0) {
        if (workdir && chdir(workdir) != 0) {
            perror("chdir");
            _exit(127);
        }
        execl(prog, prog, (char *)NULL);
        perror("execl");
        _exit(127);
    }
    return pid;
}

static void kill_child(pid_t pid) {
    if (pid <= 0) return;
    kill(pid, SIGTERM);
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 200000000L;
    nanosleep(&ts, NULL);
    kill(pid, SIGKILL);
    waitpid(pid, NULL, 0);
}

typedef struct {
    Buffer raw;
    bool saw_output;
    bool hit_eof;
    bool timed_out;
} CaptureResult;

static void capture_init(CaptureResult *cr) {
    buf_init(&cr->raw);
    cr->saw_output = false;
    cr->hit_eof = false;
    cr->timed_out = false;
}

static void capture_free(CaptureResult *cr) {
    buf_free(&cr->raw);
}

static void read_until_file_prompt(int fd, CaptureResult *cr, int timeout_ms) {
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    int elapsed = 0;
    const int slice = 100;
    while (elapsed < timeout_ms) {
        int rc = poll(&pfd, 1, slice);
        if (rc < 0) {
            if (errno == EINTR) continue;
            die("poll failed while waiting for file prompt");
        }
        if (rc == 0) {
            elapsed += slice;
        } else if (pfd.revents & (POLLIN | POLLHUP)) {
            char tmp[READ_CHUNK];
            ssize_t n = read(fd, tmp, sizeof(tmp));
            if (n == 0) { cr->hit_eof = true; return; }
            if (n < 0) {
                if (errno == EIO) { cr->hit_eof = true; return; }
                if (errno == EINTR) continue;
                die("read failed while waiting for file prompt");
            }
            buf_append(&cr->raw, tmp, (size_t)n);
            cr->saw_output = true;
            if (output_has_file_prompt(cr->raw.data)) return;
        }
    }
    cr->timed_out = true;
}

static void capture_step_output(int fd, CaptureResult *cr, int timeout_ms, int quiet_ms) {
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    int elapsed = 0;
    int quiet = 0;
    const int slice = 100;
    while (elapsed < timeout_ms) {
        int rc = poll(&pfd, 1, slice);
        if (rc < 0) {
            if (errno == EINTR) continue;
            die("poll failed while capturing step output");
        }
        if (rc == 0) {
            elapsed += slice;
            if (cr->saw_output) {
                quiet += slice;
                if (quiet >= quiet_ms) return;
            }
            continue;
        }
        if (pfd.revents & (POLLIN | POLLHUP)) {
            char tmp[READ_CHUNK];
            ssize_t n = read(fd, tmp, sizeof(tmp));
            if (n == 0) { cr->hit_eof = true; return; }
            if (n < 0) {
                if (errno == EIO) { cr->hit_eof = true; return; }
                if (errno == EINTR) continue;
                die("read failed while capturing step output");
            }
            buf_append(&cr->raw, tmp, (size_t)n);
            cr->saw_output = true;
            quiet = 0;
        }
    }
    cr->timed_out = true;
}

static bool looks_like_label_start(const char *s) {
    if (!s || !*s) return false;
    unsigned char c = (unsigned char)*s;
    if (isalpha(c)) return true;
    if (c == '(' || c == '"' || c == '[') return true;
    return false;
}

static bool parse_leading_int(const char *s, long *value, const char **after_num) {
    const char *p = s;
    while (*p == ' ' || *p == '\t') p++;
    if (!*p) return false;
    if (!isdigit((unsigned char)*p) && !((*p == '-' || *p == '+') && isdigit((unsigned char)p[1]))) return false;
    char *endp = NULL;
    long v = strtol(p, &endp, 10);
    if (endp == p) return false;
    if (!isspace((unsigned char)*endp)) return false;
    *value = v;
    *after_num = endp;
    return true;
}

static int parse_option_pairs_from_line(const char *line, OptionItem **items_out) {
    *items_out = NULL;
    const char *p = line;
    int count = 0;
    while (1) {
        long token_val = 0;
        const char *after_num = NULL;
        if (!parse_leading_int(p, &token_val, &after_num)) break;
        while (*after_num == ' ' || *after_num == '\t') after_num++;
        if (!*after_num) break;

        const char *label_start = after_num;
        const char *label_end = line + strlen(line);
        const char *q = label_start;
        while (*q) {
            if ((q == label_start || (q[0] == ' ' && q[1] == ' ')) ) {
                const char *r = q;
                int spaces = 0;
                while (*r == ' ' || *r == '\t') { spaces++; r++; }
                if (spaces >= 2) {
                    long next_val = 0;
                    const char *after_next = NULL;
                    if (parse_leading_int(r, &next_val, &after_next)) {
                        while (*after_next == ' ' || *after_next == '\t') after_next++;
                        if (looks_like_label_start(after_next)) {
                            label_end = q;
                            break;
                        }
                    }
                }
            }
            q++;
        }

        char tokbuf[64];
        snprintf(tokbuf, sizeof(tokbuf), "%ld", token_val);
        OptionItem item;
        item.token = xstrdup(tokbuf);
        item.label = trimdup_n(label_start, (size_t)(label_end - label_start));
        *items_out = (OptionItem *)xrealloc(*items_out, (size_t)(count + 1) * sizeof(OptionItem));
        (*items_out)[count++] = item;

        if (label_end >= line + strlen(line)) break;
        p = label_end;
    }
    return count;
}

static void free_option_items(OptionItem *items, int n) {
    if (!items) return;
    for (int i = 0; i < n; ++i) {
        free(items[i].token);
        free(items[i].label);
    }
    free(items);
}

static char **split_lines(const char *text, int *nlines) {
    *nlines = 0;
    if (!text) return NULL;
    int cap = 64;
    char **lines = (char **)xmalloc((size_t)cap * sizeof(char *));
    const char *p = text;
    while (*p) {
        const char *nl = strchr(p, '\n');
        size_t len = nl ? (size_t)(nl - p) : strlen(p);
        if (*nlines == cap) {
            cap *= 2;
            lines = (char **)xrealloc(lines, (size_t)cap * sizeof(char *));
        }
        lines[*nlines] = trimdup_n(p, len);
        (*nlines)++;
        if (!nl) break;
        p = nl + 1;
    }
    return lines;
}

static void free_lines(char **lines, int nlines) {
    if (!lines) return;
    for (int i = 0; i < nlines; ++i) free(lines[i]);
    free(lines);
}

static char *find_prompt_line(char **lines, int nlines) {
    char *fallback = NULL;
    for (int i = nlines - 1; i >= 0; --i) {
        if (!lines[i] || !*lines[i]) continue;
        if (!fallback) fallback = lines[i];
        if (contains_nocase(lines[i], "input") || contains_nocase(lines[i], "select") || strchr(lines[i], ':')) {
            return xstrdup(lines[i]);
        }
    }
    return fallback ? xstrdup(fallback) : NULL;
}

static void append_option(OptionItem **dst, int *count, const OptionItem *src) {
    *dst = (OptionItem *)xrealloc(*dst, (size_t)(*count + 1) * sizeof(OptionItem));
    (*dst)[*count].token = xstrdup(src->token);
    (*dst)[*count].label = xstrdup(src->label);
    (*count)++;
}

static bool looks_like_menu_title_line(const char *s) {
    if (!s || !*s) return false;
    if (contains_nocase(s, "menu") || contains_nocase(s, "available") || contains_nocase(s, "select")) return true;
    int dashes = 0, stars = 0;
    for (const char *p = s; *p; ++p) {
        if (*p == '-') dashes++;
        if (*p == '*') stars++;
    }
    return dashes >= 6 || stars >= 6;
}

static NodeState classify_output(const char *normalized, char **menu_title, char **prompt_line,
                                 OptionItem **options, int *nopt) {
    *menu_title = NULL;
    *prompt_line = NULL;
    *options = NULL;
    *nopt = 0;

    int nlines = 0;
    char **lines = split_lines(normalized, &nlines);
    if (nlines <= 0) {
        free_lines(lines, nlines);
        return NODE_UNKNOWN;
    }

    int *is_option = (int *)xmalloc((size_t)nlines * sizeof(int));
    for (int i = 0; i < nlines; ++i) {
        OptionItem *tmp = NULL;
        int ntmp = parse_option_pairs_from_line(lines[i], &tmp);
        is_option[i] = (ntmp > 0);
        free_option_items(tmp, ntmp);
    }

    int best_end = -1;
    for (int i = nlines - 1; i >= 0; --i) {
        if (is_option[i]) { best_end = i; break; }
    }

    if (best_end >= 0) {
        int best_start = best_end;
        while (best_start > 0) {
            const char *prev = lines[best_start - 1];
            if (is_option[best_start - 1] || !prev[0] || looks_like_menu_title_line(prev)) best_start--;
            else break;
        }

        for (int i = best_start; i <= best_end; ++i) {
            if (!is_option[i]) continue;
            OptionItem *tmp = NULL;
            int ntmp = parse_option_pairs_from_line(lines[i], &tmp);
            for (int k = 0; k < ntmp; ++k) append_option(options, nopt, &tmp[k]);
            free_option_items(tmp, ntmp);
        }

        for (int i = best_end; i >= best_start; --i) {
            if (!is_option[i] && looks_like_menu_title_line(lines[i])) {
                *menu_title = xstrdup(lines[i]);
                break;
            }
        }
        if (!*menu_title) {
            for (int i = best_start - 1; i >= 0; --i) {
                if (!lines[i] || !*lines[i]) continue;
                if (!is_option[i]) { *menu_title = xstrdup(lines[i]); break; }
            }
        }
        if (!*menu_title) *menu_title = xstrdup("(menu)");

        free(is_option);
        free_lines(lines, nlines);
        return NODE_MENU;
    }

    free(is_option);
    *prompt_line = find_prompt_line(lines, nlines);
    free_lines(lines, nlines);
    if (*prompt_line) {
        if (contains_nocase(*prompt_line, "input") || contains_nocase(*prompt_line, "select") || strchr(*prompt_line, ':')) return NODE_PROMPT;
        return NODE_TEXT;
    }
    return NODE_UNKNOWN;
}

static void nodestore_init(NodeStore *st) {
    st->items = NULL;
    st->count = 0;
    st->cap = 0;
}

static void nodestore_free(NodeStore *st) {
    for (int i = 0; i < st->count; ++i) {
        NodeRecord *n = &st->items[i];
        free(n->path);
        free(n->parent_path);
        free(n->input_sent);
        free(n->menu_title);
        free(n->prompt_line);
        free(n->raw_file);
        free(n->text_file);
        free(n->run_dir);
        for (int j = 0; j < n->nopt; ++j) {
            free(n->options[j].token);
            free(n->options[j].label);
        }
        free(n->options);
        for (int j = 0; j < n->n_created; ++j) free(n->created_files[j]);
        for (int j = 0; j < n->n_modified; ++j) free(n->modified_files[j]);
        free(n->created_files);
        free(n->modified_files);
    }
    free(st->items);
    st->items = NULL;
    st->count = st->cap = 0;
}

static NodeRecord *nodestore_find(NodeStore *st, const char *path) {
    for (int i = 0; i < st->count; ++i) {
        if (strcmp(st->items[i].path, path) == 0) return &st->items[i];
    }
    return NULL;
}

static NodeRecord *nodestore_add(NodeStore *st, const char *path) {
    NodeRecord *existing = nodestore_find(st, path);
    if (existing) return existing;
    if (st->count == st->cap) {
        st->cap = st->cap ? st->cap * 2 : 64;
        st->items = (NodeRecord *)xrealloc(st->items, (size_t)st->cap * sizeof(NodeRecord));
    }
    NodeRecord *n = &st->items[st->count];
    memset(n, 0, sizeof(*n));
    n->path = xstrdup(path);
    n->order = st->count;
    st->count++;
    return n;
}

static bool token_in_array(char **arr, int count, const char *s) {
    for (int i = 0; i < count; ++i) if (strcmp(arr[i], s) == 0) return true;
    return false;
}

static void merge_string_arrays(char ***dst, int *dst_n, char **src, int src_n) {
    for (int i = 0; i < src_n; ++i) {
        if (!token_in_array(*dst, *dst_n, src[i])) add_string(dst, dst_n, src[i]);
    }
}

static void nodestore_upsert(NodeStore *st, const PathSpec *pathspec,
                             NodeState state, const char *menu_title, const char *prompt_line,
                             OptionItem *options, int nopt,
                             const char *raw_file, const char *text_file,
                             const char *run_dir,
                             char **created_files, int n_created,
                             char **modified_files, int n_modified) {
    char *path = pathspec_join(pathspec);
    NodeRecord *n = nodestore_add(st, path);
    free(path);

    if (!n->parent_path) n->parent_path = pathspec_parent_string(pathspec);
    n->depth = pathspec->depth;
    if (pathspec->depth > 0 && !n->input_sent) n->input_sent = xstrdup(pathspec->items[pathspec->depth - 1].token);
    if (n->state == NODE_UNKNOWN || state == NODE_MENU || state == NODE_PROMPT || state == NODE_TEXT || state == NODE_TIMEOUT || state == NODE_EOF) n->state = state;

    if (menu_title && *menu_title && !n->menu_title) n->menu_title = xstrdup(menu_title);
    if (prompt_line && *prompt_line && !n->prompt_line) n->prompt_line = xstrdup(prompt_line);
    if (raw_file && !n->raw_file) n->raw_file = xstrdup(raw_file);
    if (text_file && !n->text_file) n->text_file = xstrdup(text_file);
    if (run_dir && !n->run_dir) n->run_dir = xstrdup(run_dir);

    if (n->nopt == 0 && nopt > 0) {
        n->options = (OptionItem *)xmalloc((size_t)nopt * sizeof(OptionItem));
        n->nopt = nopt;
        for (int i = 0; i < nopt; ++i) {
            n->options[i].token = xstrdup(options[i].token);
            n->options[i].label = xstrdup(options[i].label);
        }
    }

    merge_string_arrays(&n->created_files, &n->n_created, created_files, n_created);
    merge_string_arrays(&n->modified_files, &n->n_modified, modified_files, n_modified);
}

static void copy_file_if_exists(const char *src, const char *dst) {
    int in = open(src, O_RDONLY);
    if (in < 0) return;
    int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (out < 0) {
        close(in);
        return;
    }
    char buf[8192];
    ssize_t n;
    while ((n = read(in, buf, sizeof(buf))) > 0) {
        ssize_t off = 0;
        while (off < n) {
            ssize_t w = write(out, buf + off, (size_t)(n - off));
            if (w < 0) break;
            off += w;
        }
    }
    close(in);
    close(out);
}

static void make_run_dir(const Config *cfg, const PathSpec *terminal_path, char **run_dir_out) {
    char *path = pathspec_join(terminal_path);
    char *safe = sanitize_for_filename(path);
    size_t len = strlen(cfg->out_dir) + strlen("/runs/") + strlen(safe) + 2;
    char *dir = (char *)xmalloc(len);
    snprintf(dir, len, "%s/runs/%s", cfg->out_dir, safe);
    mkdir_p(dir);
    free(path);
    free(safe);

    char *bin_dir = path_dirname(cfg->bin_path);
    char *settings_src = (char *)xmalloc(strlen(bin_dir) + strlen("/settings.ini") + 2);
    snprintf(settings_src, strlen(bin_dir) + strlen("/settings.ini") + 2, "%s/settings.ini", bin_dir);
    char *settings_dst = (char *)xmalloc(strlen(dir) + strlen("/settings.ini") + 2);
    snprintf(settings_dst, strlen(dir) + strlen("/settings.ini") + 2, "%s/settings.ini", dir);
    copy_file_if_exists(settings_src, settings_dst);
    free(bin_dir);
    free(settings_src);
    free(settings_dst);

    *run_dir_out = dir;
}

static void free_string_array(char **arr, int n) {
    if (!arr) return;
    for (int i = 0; i < n; ++i) free(arr[i]);
    free(arr);
}

static void store_capture_as_node(NodeStore *store, const Config *cfg, const PathSpec *pathspec, const char *run_dir,
                                  const char *raw_output, const char *norm_output,
                                  char **created_files, int n_created,
                                  char **modified_files, int n_modified,
                                  bool timeout, bool hit_eof) {
    char *path = pathspec_join(pathspec);
    char *safe = sanitize_for_filename(path);
    char *steps_dir = (char *)xmalloc(strlen(cfg->out_dir) + strlen("/steps") + 2);
    snprintf(steps_dir, strlen(cfg->out_dir) + strlen("/steps") + 2, "%s/steps", cfg->out_dir);
    mkdir_p(steps_dir);

    char *raw_file = (char *)xmalloc(strlen(steps_dir) + strlen(safe) + 16);
    char *text_file = (char *)xmalloc(strlen(steps_dir) + strlen(safe) + 16);
    snprintf(raw_file, strlen(steps_dir) + strlen(safe) + 16, "%s/%s.raw.txt", steps_dir, safe);
    snprintf(text_file, strlen(steps_dir) + strlen(safe) + 16, "%s/%s.txt", steps_dir, safe);
    write_text_file(raw_file, raw_output);
    write_text_file(text_file, norm_output);

    char *menu_title = NULL;
    char *prompt_line = NULL;
    OptionItem *options = NULL;
    int nopt = 0;
    NodeState state = classify_output(norm_output, &menu_title, &prompt_line, &options, &nopt);
    if (timeout) state = NODE_TIMEOUT;
    else if (hit_eof) state = (state == NODE_UNKNOWN ? NODE_EOF : state);
    else if (state == NODE_UNKNOWN && norm_output && *norm_output) state = NODE_TEXT;

    nodestore_upsert(store, pathspec, state, menu_title, prompt_line, options, nopt,
                     raw_file, text_file, run_dir,
                     created_files, n_created, modified_files, n_modified);

    free(path);
    free(safe);
    free(steps_dir);
    free(raw_file);
    free(text_file);
    free(menu_title);
    free(prompt_line);
    free_option_items(options, nopt);
}

static void probe_terminal_path(const Config *cfg, NodeStore *store, const PathSpec *terminal_path) {
    char *run_dir = NULL;
    make_run_dir(cfg, terminal_path, &run_dir);

    int mfd = -1;
    pid_t pid = spawn_under_pty(cfg->bin_path, run_dir, &mfd);

    CaptureResult boot;
    capture_init(&boot);
    read_until_file_prompt(mfd, &boot, cfg->boot_timeout_ms);
    if (boot.timed_out || boot.hit_eof || !output_has_file_prompt(boot.raw.data)) {
        PathSpec root;
        root.depth = 0;
        char *norm = normalize_tty_text(boot.raw.data);
        store_capture_as_node(store, cfg, &root, run_dir, boot.raw.data, norm, NULL, 0, NULL, 0, boot.timed_out, boot.hit_eof);
        free(norm);
        capture_free(&boot);
        kill_child(pid);
        free(run_dir);
        return;
    }
    capture_free(&boot);

    if (send_line(mfd, cfg->input_path) < 0) {
        kill_child(pid);
        free(run_dir);
        die("failed to send input path");
    }

    PathSpec current;
    current.depth = 0;

    for (int step = 0; step <= terminal_path->depth; ++step) {
        DirSnapshot before, after;
        dirsnap_init(&before);
        dirsnap_init(&after);
        capture_dirsnapshot(run_dir, &before);

        CaptureResult cap;
        capture_init(&cap);
        capture_step_output(mfd, &cap, cfg->step_timeout_ms, cfg->quiet_ms);
        capture_dirsnapshot(run_dir, &after);

        char **created = NULL, **modified = NULL;
        int n_created = 0, n_modified = 0;
        diff_dirs(&before, &after, &created, &n_created, &modified, &n_modified);

        char *norm = normalize_tty_text(cap.raw.data);
        store_capture_as_node(store, cfg, &current, run_dir, cap.raw.data, norm,
                              created, n_created, modified, n_modified,
                              cap.timed_out, cap.hit_eof);
        free(norm);
        capture_free(&cap);
        dirsnap_free(&before);
        dirsnap_free(&after);
        free_string_array(created, n_created);
        free_string_array(modified, n_modified);

        if (step == terminal_path->depth) break;
        const char *token = terminal_path->items[step].token;
        if (send_line(mfd, token) < 0) {
            kill_child(pid);
            free(run_dir);
            die("failed to send token: %s", token);
        }
        pathspec_push(&current, token);
    }

    kill_child(pid);
    free(run_dir);
}

static void collect_children_indices(const NodeStore *st, const char *parent_path, int **idxs, int *nidx) {
    *idxs = NULL;
    *nidx = 0;
    for (int i = 0; i < st->count; ++i) {
        const NodeRecord *r = &st->items[i];
        bool is_child = false;
        if (!parent_path) is_child = (r->depth == 0);
        else is_child = (r->parent_path && strcmp(r->parent_path, parent_path) == 0);
        if (is_child) {
            *idxs = (int *)xrealloc(*idxs, (size_t)(*nidx + 1) * sizeof(int));
            (*idxs)[(*nidx)++] = i;
        }
    }
}

static const NodeStore *g_sort_store = NULL;

static int cmp_node_indices_qsort(const void *a, const void *b) {
    const NodeStore *st = g_sort_store;
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    const NodeRecord *na = &st->items[ia];
    const NodeRecord *nb = &st->items[ib];
    return strcmp(na->path, nb->path);
}

static void write_tree_rec(FILE *fp, const NodeStore *st, const char *parent_path, int indent) {
    int *idxs = NULL;
    int nidx = 0;
    collect_children_indices(st, parent_path, &idxs, &nidx);
    if (nidx > 1) { g_sort_store = st; qsort(idxs, (size_t)nidx, sizeof(int), cmp_node_indices_qsort); g_sort_store = NULL; }
    for (int i = 0; i < nidx; ++i) {
        const NodeRecord *n = &st->items[idxs[i]];
        for (int sp = 0; sp < indent; ++sp) fputs("  ", fp);
        fprintf(fp, "[%s] %s", n->path[0] ? n->path : "root", state_name(n->state));
        if (n->menu_title && *n->menu_title) fprintf(fp, " | %s", n->menu_title);
        else if (n->prompt_line && *n->prompt_line) fprintf(fp, " | %s", n->prompt_line);
        fprintf(fp, "\n");
        for (int sp = 0; sp < indent + 1; ++sp) fputs("  ", fp);
        fprintf(fp, "text: %s\n", n->text_file ? n->text_file : "");
        if (n->n_created > 0 || n->n_modified > 0) {
            for (int sp = 0; sp < indent + 1; ++sp) fputs("  ", fp);
            fputs("files:", fp);
            for (int j = 0; j < n->n_created; ++j) fprintf(fp, " +%s", n->created_files[j]);
            for (int j = 0; j < n->n_modified; ++j) fprintf(fp, " ~%s", n->modified_files[j]);
            fputc('\n', fp);
        }
        if (n->nopt > 0) {
            for (int j = 0; j < n->nopt; ++j) {
                for (int sp = 0; sp < indent + 1; ++sp) fputs("  ", fp);
                fprintf(fp, "%s -> %s\n", n->options[j].token, n->options[j].label);
            }
        }
        write_tree_rec(fp, st, n->path, indent + 1);
    }
    free(idxs);
}

static void write_manifest(const Config *cfg, const NodeStore *store) {
    char manifest_path[4096];
    snprintf(manifest_path, sizeof(manifest_path), "%s/manifest.json", cfg->out_dir);
    FILE *fp = fopen(manifest_path, "wb");
    if (!fp) die("cannot open manifest for write");

    char *start_path = pathspec_join(&cfg->start_path);
    fputs("{\n", fp);
    fputs("  \"tool\": \"multiwfn_probe\",\n", fp);
    fputs("  \"bin_path\": ", fp); append_json_string(fp, cfg->bin_path); fputs(",\n", fp);
    fputs("  \"input_path\": ", fp); append_json_string(fp, cfg->input_path); fputs(",\n", fp);
    fputs("  \"out_dir\": ", fp); append_json_string(fp, cfg->out_dir); fputs(",\n", fp);
    fputs("  \"start_path\": ", fp); append_json_string(fp, start_path); fputs(",\n", fp);
    fprintf(fp, "  \"expand_depth\": %d,\n", cfg->expand_depth);
    fprintf(fp, "  \"boot_timeout_ms\": %d,\n", cfg->boot_timeout_ms);
    fprintf(fp, "  \"step_timeout_ms\": %d,\n", cfg->step_timeout_ms);
    fprintf(fp, "  \"quiet_ms\": %d,\n", cfg->quiet_ms);
    fputs("  \"nodes\": [\n", fp);
    for (int i = 0; i < store->count; ++i) {
        const NodeRecord *n = &store->items[i];
        fputs("    {\n", fp);
        fputs("      \"path\": ", fp); append_json_string(fp, n->path); fputs(",\n", fp);
        fputs("      \"parent_path\": ", fp); if (n->parent_path) append_json_string(fp, n->parent_path); else fputs("null", fp); fputs(",\n", fp);
        fprintf(fp, "      \"depth\": %d,\n", n->depth);
        fputs("      \"input_sent\": ", fp); if (n->input_sent) append_json_string(fp, n->input_sent); else fputs("null", fp); fputs(",\n", fp);
        fputs("      \"state\": ", fp); append_json_string(fp, state_name(n->state)); fputs(",\n", fp);
        fputs("      \"menu_title\": ", fp); if (n->menu_title) append_json_string(fp, n->menu_title); else fputs("null", fp); fputs(",\n", fp);
        fputs("      \"prompt_line\": ", fp); if (n->prompt_line) append_json_string(fp, n->prompt_line); else fputs("null", fp); fputs(",\n", fp);
        fputs("      \"raw_file\": ", fp); if (n->raw_file) append_json_string(fp, n->raw_file); else fputs("null", fp); fputs(",\n", fp);
        fputs("      \"text_file\": ", fp); if (n->text_file) append_json_string(fp, n->text_file); else fputs("null", fp); fputs(",\n", fp);
        fputs("      \"run_dir\": ", fp); if (n->run_dir) append_json_string(fp, n->run_dir); else fputs("null", fp); fputs(",\n", fp);
        fputs("      \"created_files\": [", fp);
        for (int j = 0; j < n->n_created; ++j) {
            if (j) fputs(", ", fp);
            append_json_string(fp, n->created_files[j]);
        }
        fputs("],\n", fp);
        fputs("      \"modified_files\": [", fp);
        for (int j = 0; j < n->n_modified; ++j) {
            if (j) fputs(", ", fp);
            append_json_string(fp, n->modified_files[j]);
        }
        fputs("],\n", fp);
        fputs("      \"options\": [\n", fp);
        for (int j = 0; j < n->nopt; ++j) {
            fputs("        {\"token\": ", fp); append_json_string(fp, n->options[j].token);
            fputs(", \"label\": ", fp); append_json_string(fp, n->options[j].label);
            fputs("}", fp);
            if (j + 1 < n->nopt) fputs(",", fp);
            fputs("\n", fp);
        }
        fputs("      ]\n", fp);
        fputs("    }", fp);
        if (i + 1 < store->count) fputs(",", fp);
        fputs("\n", fp);
    }
    fputs("  ]\n", fp);
    fputs("}\n", fp);
    fclose(fp);

    char tree_path[4096];
    snprintf(tree_path, sizeof(tree_path), "%s/tree.txt", cfg->out_dir);
    fp = fopen(tree_path, "wb");
    if (!fp) die("cannot open tree.txt for write");
    fprintf(fp, "start_path=%s\nexpand_depth=%d\n\n", start_path ? start_path : "", cfg->expand_depth);
    write_tree_rec(fp, store, NULL, 0);
    fclose(fp);
    free(start_path);
}

static void usage(FILE *fp) {
    fprintf(fp,
        "Usage: multiwfn_probe --bin <Multiwfn_noGUI> --input <wavefunction-file> [options]\n"
        "\n"
        "Options:\n"
        "  --path <a|b|c>          Replay this path before stopping. Default: root\n"
        "  --depth <N>             Expand N levels below the final path. Default: 0\n"
        "  --out-dir <dir>         Output directory. Default: ./mwfn_probe_out\n"
        "  --boot-timeout-ms <N>   Timeout for startup/file prompt. Default: %d\n"
        "  --step-timeout-ms <N>   Timeout for each step after sending input. Default: %d\n"
        "  --quiet-ms <N>          Consider a step finished after N ms of silence. Default: %d\n"
        "  --max-nodes <N>         Stop after recording this many nodes. Default: %d\n"
        "  --help                  Show this help\n"
        "\n"
        "Examples:\n"
        "  multiwfn_probe --bin ./Multiwfn_noGUI --input ./4-nitroaniline.fch\n"
        "  multiwfn_probe --bin ./Multiwfn_noGUI --input ./4-nitroaniline.fch --path 5|1\n"
        "  multiwfn_probe --bin ./Multiwfn_noGUI --input ./4-nitroaniline.fch --path 5|1 --depth 1\n",
        DEFAULT_BOOT_TIMEOUT_MS, DEFAULT_STEP_TIMEOUT_MS, DEFAULT_QUIET_MS, DEFAULT_MAX_NODES);
}

static void parse_args(Config *cfg, int argc, char **argv) {
    memset(cfg, 0, sizeof(*cfg));
    cfg->out_dir = xstrdup("./mwfn_probe_out");
    cfg->expand_depth = 0;
    cfg->boot_timeout_ms = DEFAULT_BOOT_TIMEOUT_MS;
    cfg->step_timeout_ms = DEFAULT_STEP_TIMEOUT_MS;
    cfg->quiet_ms = DEFAULT_QUIET_MS;
    cfg->max_nodes = DEFAULT_MAX_NODES;
    cfg->start_path.depth = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--bin") == 0 && i + 1 < argc) cfg->bin_path = xstrdup(argv[++i]);
        else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) cfg->input_path = xstrdup(argv[++i]);
        else if (strcmp(argv[i], "--path") == 0 && i + 1 < argc) parse_pathspec(&cfg->start_path, argv[++i]);
        else if (strcmp(argv[i], "--depth") == 0 && i + 1 < argc) cfg->expand_depth = atoi(argv[++i]);
        else if (strcmp(argv[i], "--out-dir") == 0 && i + 1 < argc) { free(cfg->out_dir); cfg->out_dir = xstrdup(argv[++i]); }
        else if (strcmp(argv[i], "--boot-timeout-ms") == 0 && i + 1 < argc) cfg->boot_timeout_ms = atoi(argv[++i]);
        else if (strcmp(argv[i], "--step-timeout-ms") == 0 && i + 1 < argc) cfg->step_timeout_ms = atoi(argv[++i]);
        else if (strcmp(argv[i], "--quiet-ms") == 0 && i + 1 < argc) cfg->quiet_ms = atoi(argv[++i]);
        else if (strcmp(argv[i], "--max-nodes") == 0 && i + 1 < argc) cfg->max_nodes = atoi(argv[++i]);
        else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(stdout);
            exit(0);
        } else {
            usage(stderr);
            die("unknown or incomplete argument: %s", argv[i]);
        }
    }
    if (!cfg->bin_path || !cfg->input_path) {
        usage(stderr);
        die("--bin and --input are required");
    }
    char *resolved_bin = resolve_path(cfg->bin_path);
    char *resolved_input = resolve_path(cfg->input_path);
    free(cfg->bin_path);
    free(cfg->input_path);
    cfg->bin_path = resolved_bin;
    cfg->input_path = resolved_input;
    if (cfg->expand_depth < 0) cfg->expand_depth = 0;
    if (cfg->max_nodes <= 0) cfg->max_nodes = DEFAULT_MAX_NODES;
    if (cfg->quiet_ms <= 0) cfg->quiet_ms = DEFAULT_QUIET_MS;
    mkdir_p(cfg->out_dir);
    char runs_dir[4096];
    snprintf(runs_dir, sizeof(runs_dir), "%s/runs", cfg->out_dir);
    mkdir_p(runs_dir);
    char steps_dir[4096];
    snprintf(steps_dir, sizeof(steps_dir), "%s/steps", cfg->out_dir);
    mkdir_p(steps_dir);
}

static void free_config(Config *cfg) {
    free(cfg->bin_path);
    free(cfg->input_path);
    free(cfg->out_dir);
}

static void explore(const Config *cfg, NodeStore *store) {
    QueueItem *queue = NULL;
    int qh = 0, qt = 0, qcap = 0;
    if (qcap == qt) {
        qcap = 32;
        queue = (QueueItem *)xrealloc(queue, (size_t)qcap * sizeof(QueueItem));
    }
    pathspec_copy(&queue[qt].path, &cfg->start_path);
    queue[qt].rel_depth = 0;
    qt++;

    while (qh < qt) {
        QueueItem cur = queue[qh++];
        if (store->count >= cfg->max_nodes) break;

        char *cur_path = pathspec_join(&cur.path);
        NodeRecord *existing = nodestore_find(store, cur_path);
        bool need_probe = true;
        if (existing && (existing->state != NODE_UNKNOWN)) need_probe = false;
        if (need_probe) probe_terminal_path(cfg, store, &cur.path);
        NodeRecord *node = nodestore_find(store, cur_path);

        if (node && cur.rel_depth < cfg->expand_depth && node->state == NODE_MENU && node->nopt > 0) {
            for (int i = 0; i < node->nopt; ++i) {
                if (qt == qcap) {
                    qcap *= 2;
                    queue = (QueueItem *)xrealloc(queue, (size_t)qcap * sizeof(QueueItem));
                }
                PathSpec child;
                pathspec_copy(&child, &cur.path);
                pathspec_push(&child, node->options[i].token);
                bool seen = false;
                for (int j = 0; j < qt; ++j) {
                    if (queue[j].rel_depth == cur.rel_depth + 1 && pathspec_equal(&queue[j].path, &child)) { seen = true; break; }
                }
                char *child_path = pathspec_join(&child);
                if (nodestore_find(store, child_path)) seen = true;
                free(child_path);
                if (!seen) {
                    pathspec_copy(&queue[qt].path, &child);
                    queue[qt].rel_depth = cur.rel_depth + 1;
                    qt++;
                }
            }
        }
        free(cur_path);
    }
    free(queue);
}

int main(int argc, char **argv) {
    Config cfg;
    parse_args(&cfg, argc, argv);
    NodeStore store;
    nodestore_init(&store);

    explore(&cfg, &store);
    write_manifest(&cfg, &store);

    printf("Wrote %d nodes to %s/manifest.json and %s/tree.txt\n", store.count, cfg.out_dir, cfg.out_dir);

    nodestore_free(&store);
    free_config(&cfg);
    return 0;
}
