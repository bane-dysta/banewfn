# BaneWfn BWC generator reference pack

Read in this order when generating a `.bwc` script:

1. `bwc-style-guide.md` - the user's preferred BWC structure, naming, shell, VMD, and surface-analysis patterns.
2. `banewfn-dsl.md` - compact DSL syntax and execution behavior.
3. `module-index.md` - available BaneWfn modules, sections, placeholders, and uploaded examples.
4. `examples/user-bwc/` - concrete user-provided scripts to imitate.
5. `conf/<module>.conf` - exact menu sequence and placeholders for inline packaging.
6. `banewfn-wiki/` - upstream manual pages from the provided BaneWfn source.
7. `multiwfn-manual/` - split Multiwfn manual references copied from the uploaded Multiwfn skill; consult only when a new raw menu path or new `.conf` section is needed.

Use `scripts/generate_bwc.py` to append inline conf and `scripts/validate_bwc.py` to perform static checks before giving the script to the user.

## Generated-at-release references

The released skill package contains a few reference directories that are not maintained here directly:

- `conf/` is copied from the repository root `conf/` directory.
- `banewfn-wiki/` is copied from the repository root `wiki/` directory.
- `examples/user-bwc/` is copied from `scripts/bwc/` when present.

Run `scripts/release/stage_skill.sh` from the repository root to build the same staged skill package locally.
