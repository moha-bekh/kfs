# KFS — Kernel From Scratch

Projet pédagogique de kernel i386 en C et assembleur. Le démarrage utilise GRUB et Multiboot 2 ; l'affichage texte VGA et la console sont en cours de développement.

## Dépendances

- `make`, un compilateur C capable de produire du code i386, `nasm` et GNU `ld`
- GRUB (`grub-mkrescue`, modules `i386-pc`, `grub-file`), `xorriso` et `mtools`
- QEMU x86 avec interface GTK ; GDB pour le débogage

Sur Arch Linux, les paquets utilisés sont `make`, `gcc`, `binutils`, `nasm`, `grub`, `xorriso`, `mtools`, `qemu-system-x86`, `qemu-ui-gtk` et `gdb`.

## Commandes

```sh
make build            # compiler et lier build/kfs.elf
make                  # construire l'image et lancer QEMU
make run              # construire l'image si nécessaire et lancer QEMU
make check-multiboot  # vérifier l'ELF déjà construit
make gdb-server       # lancer QEMU en pause pour une connexion GDB
make clean            # supprimer build/
make fclean           # supprimer aussi kfs.iso
```

Pour déboguer, lancer `make gdb-server`, puis dans un autre terminal :

```sh
gdb build/kfs.elf
```

Dans GDB : `target remote localhost:1234`, puis par exemple `break _start` et `continue`.

## Organisation

- `src/boot/` : point d'entrée assembleur et en-tête Multiboot 2
- `src/kfs/`, `src/console/`, `src/drivers/` : kernel, console et backend VGA
- `include/` : headers ; `link/` : linker script ; `grub/` : configuration GRUB
- `TODO.md` : objectifs ; `docs/` : suivi et consignes d'apprentissage
- `build/` : fichiers intermédiaires générés ; `kfs.iso` : image générée pour QEMU
