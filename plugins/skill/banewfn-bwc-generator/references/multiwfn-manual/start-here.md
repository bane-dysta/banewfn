# Start here

Use this file when you need the fastest possible route to the right part of the Multiwfn manual.

## Fast decision tree

### 1) The user asks **what method should I use**
Do **not** jump directly into a chapter 3 function file unless the request is already very specific.
Read these overview files first:

- `manual/special-topics/4-a-3-overview-of-methods-for-studying-aromaticity.md`
- `manual/special-topics/4-a-4-overview-of-methods-for-predicting-reactive-sites.md`
- `manual/special-topics/4-a-5-overview-of-methods-for-studying-weak-interactions.md`
- `manual/special-topics/4-a-11-overview-of-methods-for-studying-chemical-bonds.md`
- `manual/special-topics/4-a-12-overview-of-methods-for-analyzing-electron-excitation.md`

### 2) The user asks **how to do a specific Multiwfn function**
Go to chapter 3 function files first:

- browse `manual/functions/README.md`
- then open the matching `manual/functions/3-...md` file

These files are the best place for:

- exact menu number
- option number
- theory section
- input file section
- parameter and option descriptions

### 3) The user asks for **a worked example / step-by-step workflow**
Go to chapter 4 tutorial files first:

- browse `manual/tutorials/README.md`
- then open the matching `manual/tutorials/4-...md` file

Use tutorial files when the user says things like:

- “怎么一步一步做”
- “有没有例子”
- “给我一个可照着跑的流程”
- “这类分析在手册里哪里有实战示例”

### 4) The user asks about **installation, prerequisites, or input files**
Go to the general files first:

- `manual/front-matter.md`
- `manual/general/2-1-install.md`
- `manual/general/2-2-using-multiwfn.md`
- `manual/general/2-5-input-files-and-wavefunction-types.md`
- `manual/general/2-9-analysis-of-periodic-systems.md`

If the user only says “这个文件能不能做某分析”, `2-5-input-files-and-wavefunction-types.md` is the first file to open.

### 5) The user asks about **silent mode, batch mode, CLI技巧, 绘图技巧**
Go to chapter 5 skill files first:

- `manual/skills/README.md`

Especially:

- `manual/skills/5-2-running-multiwfn-in-silent-mode.md`
- `manual/skills/5-3-running-multiwfn-in-batch-mode.md`
- `manual/skills/5-8-some-tips-on-achieving-ideal-drawing-quality.md`

### 6) The user asks about **technical internals or appendices**
Go to chapter 6 appendix files first:

- `manual/appendix/README.md`

## High-value files for weak or fuzzy queries

When the user’s wording is broad, ambiguous, or method-selection-oriented, these files usually give the best first answer:

- `manual/front-matter.md`
- `manual/general/2-5-input-files-and-wavefunction-types.md`
- `manual/functions/README.md`
- `manual/tutorials/README.md`
- `manual/special-topics/README.md`
- `manual/manual-outline.md`

## Safe default answer structure

When you answer a user, prefer this order:

1. **Best entry point** - say which manual area to read first
2. **Exact section title(s)** - preserve the original English section names
3. **Menu / option number** - if the section title contains it
4. **Why it matches** - explain the intent-to-section mapping
5. **Input file / prerequisite reminder** - when relevant
6. **Nearby alternatives** - only if multiple methods are plausible

## Important warning

Do not invent a “best method” if the manual overview files clearly present multiple alternatives.
When the problem is about **method selection**, overview files in `manual/special-topics/` are often more reliable than jumping straight into one function section.
