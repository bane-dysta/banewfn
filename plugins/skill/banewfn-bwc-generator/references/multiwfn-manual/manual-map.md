# Manual map

This file explains **what each major chapter is for**, so you do not confuse method overviews, function documentation, and tutorials.

## `manual/front-matter.md`
Read this when you need the manual's own warnings, support channels, recommended reading order, and platform caveats.
This is the best place for onboarding context.

## Chapter 1 and Chapter 2: `manual/general/`
Use these files for broad orientation and prerequisites.

Main roles:

- what Multiwfn is
- how to install and start it
- what files it can read
- what real-space functions mean
- how periodic systems are handled
- what graphic formats and image sizes are supported

This area is about **preconditions and background**, not detailed menu usage.

## Chapter 3: `manual/functions/`
This is the **core function reference**.
Use it when the user already knows roughly what they want to do and needs the exact Multiwfn function.

Main roles:

- menu number and suboption number
- theory and definitions
- input file requirements
- parameters, options, post-processing menus

If the user asks “Multiwfn 里哪个主功能号/子选项号做这个”, chapter 3 is usually the right place.

## Chapter 4: `manual/tutorials/`
This is the **example and workflow chapter**.
Use it when the user wants a step-by-step demonstration, not only a function definition.

Main roles:

- practical examples
- reproducible workflows
- file preparation in context
- plotting and post-processing examples

If the user asks “怎么具体做” or “有没有例子”, chapter 4 is usually better than chapter 3.

## Chapter 4.A: `manual/special-topics/`
This is the **method-selection and advanced-topic layer**.
Use it when the user has a scientific goal but has not yet chosen the most suitable Multiwfn method.

High-value overview topics:

- aromaticity
- reactive sites
- weak interactions
- chemical bonds
- electron excitation

When the user asks “我想研究 X, 应该看哪种方法”, read chapter 4.A first, then route to chapter 3 or chapter 4.

## Chapter 5: `manual/skills/`
This is the **operational tricks** chapter.
It is not about chemistry methods themselves, but about using Multiwfn more effectively.

Main roles:

- silent mode
- batch mode
- command-line tricks
- copying outputs
- drawing-quality tips

## Chapter 6: `manual/appendix/`
This is the **technical appendix**.
Use it for low-level details, implementation notes, Gaussian environment setup, pseudopotential details, and sanity checks.

## Reliable routing rule

For most user requests, route in this order:

1. broad overview or method selection -> chapter 4.A
2. exact function definition and options -> chapter 3
3. concrete example workflow -> chapter 4
4. prerequisites and file compatibility -> chapter 2
5. automation and practical CLI tricks -> chapter 5
6. low-level technical detail -> chapter 6
