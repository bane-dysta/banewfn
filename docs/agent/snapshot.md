- 使用传统目录diff时，应比较“干净baseline目录”和“当前工作目录”，并使用`-N`确保新增文本文件不会被遗漏，例如：

```sh
diff -ruN \
  --exclude='.git' \
  --exclude='build' \
  --exclude='cmake-build-*' \
  <baseline-directory>/ \
  <working-directory>/ \
  > /mnt/data/<project>-WIP-<sequence>.patch
```

- 传统`diff -ruN`适合保存文本改动，但不能可靠承载二进制内容、文件模式、符号链接和某些重命名语义；存在这些变化时必须同时提供源码压缩包，或改用支持这些变化的Git patch。
- 在Git仓库中选择Git patch时，**不可直接使用普通`git diff > snapshot.patch`后便假定快照完整**，因为它会静默遗漏未跟踪的新文件。应先检查未跟踪文件，并通过intent-to-add或其他等价方式将必要新增文件纳入diff，例如：

```sh
git status --short --untracked-files=all
git add -N .
git diff --binary --full-index --no-ext-diff > /mnt/data/<project>-WIP-<sequence>.patch
```

- 如果项目需要纳入的新增文件被ignore规则命中，应检查ignored文件；采用Git patch时可对明确需要交付的文件使用`git add -f -N <path>`。无论采用Git diff、传统目录diff还是源码包，都不得让新增源码因ignore规则或遗漏检查而静默缺失。
- 源码目录不是Git仓库时，**不需要为了生成WIP snapshot而强行初始化Git仓库**；可直接使用`diff -ruN`和源码压缩包。只有final patch仍按正式交付要求使用Git unified diff。
- 源码快照应排除`.git`、`build/`、`cmake-build-*`、对象文件、静态/动态库以及临时日志。例如：

```sh
tar \
  --exclude='.git' \
  --exclude='build' \
  --exclude='cmake-build-*' \
  --exclude='*.o' \
  --exclude='*.a' \
  --exclude='*.so' \
  -czf /mnt/data/<project>-WIP-<sequence>.tgz \
  <project-directory>
```

- 每次snapshot交付时必须说明：它是WIP还是final、所用baseline、是否包含新增文件、当前构建/测试状态、已知失败和未完成事项。
- WIP snapshot不要求通过构建或测试。若交付的是Git patch，可尽可能运行`git apply --check`；若交付的是传统目录diff，可尽可能在干净baseline副本上运行`patch --dry-run`。这些检查用于发现明显损坏，不是WIP交付前置条件。最终patch仍须按正式交付要求完成干净树应用、构建和测试验证。