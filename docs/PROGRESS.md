# KFS — Learning Progress

> Update this file manually, or ask an AI to propose a change and approve it. Do not interpret this template as a claim that any task is complete.

## Current milestone
- Reprise (6 novembre 2026) : KFS-1, console texte et historique circulaire en cours. Tutorat en français ; le learner implémente, l'assistant explique et review sans patch par défaut.
- Vérifié par le learner : ISO BIOS GRUB Multiboot 2 de 3 442 688 octets ; sous QEMU/GDB, breakpoints atteints dans `_start` puis `main`. Affichage VGA `42` observé. Les sujets KFS-4 et KFS-5 ont été partagés en conversation ; objectif jusqu'à KFS-5 avec bonus.
- Architecture choisie : backend VGA séparé de la console ; cinq screens statiques, chacun avec 250 lignes de 80 cellules et une vue de 25 lignes. Le curseur d'écriture et la vue sont distincts. Scrollback borné : suivre les nouvelles sorties seulement si la vue est en bas ; rester en historique sinon ; revenir en bas à la saisie utilisateur. À terme, explorer aussi une image disque brute et éventuellement un multiplexeur.
- État actuel : `console_putc` écrit dans le buffer ; `\n` appelle `screen_new_line` puis retourne. `console_render_screen` dessine 25 lignes avec modulo mais ne tient pas encore compte de `line_count`. Le dernier test signalait un glyphe parasite avant le deuxième `42` ; la correction du retour après `\n` a été écrite, sans validation rapportée depuis.
- Prochaine review : dans `screen_new_line`, la boucle d'effacement utilise `cursor.col` qui vient d'être mis à 0, donc ne s'exécute jamais ; `line_count` augmente sans borne ; le calcul de retour et l'avancement de `line_old` doivent être revus lorsque le ring est plein, ainsi que le suivi de la vue et l'initialisation des lignes. Guider le learner avec un petit exemple avant de modifier le code.
- Git : commit local `375d949` créé précédemment ; push HTTPS échoué faute d'authentification. De nombreux changements sont non commités ; ne pas les écraser.
- Plan : construire manuellement une ISO GRUB/Multiboot 2 et vérifier le démarrage sous QEMU/GDB ; ensuite préparer une image disque brute pour apprendre le partitionnement et l'installation de GRUB ; automatiser les deux après compréhension.
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
