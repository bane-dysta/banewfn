# Interpreting probe output

## Goal

Convert `tree.txt`, `manifest.json`, and `steps/*.txt` into a user-facing Multiwfn operating procedure.

## Read order

1. Read `tree.txt` to find the stable numbered menu path.
2. Open the corresponding `steps/*.txt` files to capture prompts and printed results.
3. Use `manifest.json` only when you need structured fields such as node state, created files, or exact text-file paths.

## What counts as the workflow path

Only numbered menu selections belong in the compact path. Free-form prompts do not.

Example:

- Correct: `18 -> 1 -> [output file] -> [state index] -> 1 -> [grid quality]`
- Incorrect: pretending every prompt is a numbered menu choice.

## What to extract from prompts

Capture the exact kind of information Multiwfn requests, such as:

- excited-state output file
- excited-state index
- fragment definition
- orbital range
- grid quality
- export filename

## What to extract from result text

Capture:

- quantitative indices
- generated file names
- follow-up export options
- warnings or assumptions

## Recommended final answer format

1. One-sentence summary
2. Exact menu path
3. Prompt sequence
4. Step-by-step instructions
5. Important notes and exports
