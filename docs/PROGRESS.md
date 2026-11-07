# KFS — Learning Progress

> Update this file manually, or ask an AI to propose a change and approve it. Do not interpret this template as a claim that any task is complete.

## Current milestone
- Resumption (November 6, 2026): KFS-1; text console and circular history in progress. Tutoring in French; the learner implements, the assistant explains and reviews without patching by default.
- Verified by the learner: BIOS GRUB Multiboot 2 ISO of 3,442,688 bytes; QEMU/GDB breakpoints reached `_start` and `main`. VGA display of `42` observed. The KFS-4 and KFS-5 subjects were shared in conversation; the goal is to reach KFS-5 with bonuses.
- Chosen architecture: VGA backend separate from the console; five static screens, each with 250 lines of 80 cells and a view of 25 lines. The write cursor and view are separate. Bounded scrollback: follow new output only when the view is at the bottom; remain in history otherwise; return to the bottom on user input. Later, explore a raw disk image and possibly a multiplexer.
- Current state: `console_putc` writes to the buffer; `\n` calls `screen_new_line` and returns. `console_render_screen` draws 25 lines using modulo but does not yet account for `line_count`. The last reported test showed an extra glyph before the second `42`; the return after `\n` was added, with no subsequent validation reported.
- Next review: in `screen_new_line`, the clearing loop uses `cursor.col` immediately after it is reset to zero, so it never executes; `line_count` increases without a bound; review wraparound and advancement of `line_old` when the ring is full, along with view tracking and line initialization. Guide the learner through a small example before changing code.
- Plan: build a raw disk image to learn partitioning and GRUB installation, then automate both image workflows after understanding them.
- KFS assignment / subject: **to fill in**
- Current branch / commit: **to fill in**
- Target architecture and CPU mode: **verify from subject and build**
- Boot path / bootloader: **verify**
- Cross-compiler, emulator, debugger: **to fill in**

## What currently works (observed)
- [ ] Describe a successful boot and the evidence (serial/VGA/logs/GDB)

## Concepts I can explain independently
- (add concept, concrete evidence or explanation)

## Concepts I am learning
- (add concept, open question, reference)

## Current blocker
- Symptom:
- Expected behavior:
- Observed behavior:
- Exact reproduction command:
- Evidence (registers/logs/GDB/linker map):
- Hypotheses tested:
- Next smallest experiment:

## Design decisions / trade-offs
| Date | Decision | Reason | Alternatives |
| --- | --- | --- | --- |

## Learning journal
### YYYY-MM-DD
- Implemented myself:
- Understood:
- Still unclear:
- Next step:
