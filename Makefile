.PHONY: all run build clean fclean re check-multiboot gdb-server

BUILD_DIR = build
BOOT_DIR = $(BUILD_DIR)/boot
ISO_DIR = $(BUILD_DIR)/iso
GRUB_DIR = $(ISO_DIR)/boot/grub

ELF = $(BUILD_DIR)/kfs.elf
ISO = kfs.iso

GRUB_CONFIG = grub/grub.cfg
GRUB_MKRESCUE_FLAGS = \
	--directory=/usr/lib/grub/i386-pc \
	--locales= --themes= --fonts=

LINKER_SCRIPT = link/linker.ld

C_SRC = \
	src/kfs/main.c \
	src/console/console.c \
	src/drivers/vga.c

ASM_SRC = src/boot/boot.s

CFLAGS = \
	-m32 \
	-Wall -Wextra -Werror \
	-fno-builtin \
	-fno-stack-protector \
	-ffreestanding \
	-fno-pie

OBJ = \
	$(patsubst src/%.c, $(BUILD_DIR)/%.o, $(C_SRC)) \
	$(patsubst src/%.s, $(BUILD_DIR)/%.o, $(ASM_SRC))

all: $(ISO) run

run: $(ISO)
	qemu-system-i386 -display gtk -cdrom $(ISO)

build: $(ELF)

$(ISO): $(ELF) $(GRUB_CONFIG)
	mkdir -p $(GRUB_DIR)
	cp $(GRUB_CONFIG) $(GRUB_DIR)
	cp $(ELF) $(ISO_DIR)/boot/kfs.elf
	grub-mkrescue $(GRUB_MKRESCUE_FLAGS) $(ISO_DIR) -o $(ISO)

$(ELF): $(OBJ) $(LINKER_SCRIPT)
	ld -m elf_i386 $(OBJ) -T link/linker.ld -o $@

$(BUILD_DIR)/%.o: src/%.s
	mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(BUILD_DIR)/%.o: src/%.c
	mkdir -p $(dir $@)
	cc $(CFLAGS) -I include -c $< -o $@

check-multiboot:
	grub-file --is-x86-multiboot2 $(ELF)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -rf $(ISO)

re: fclean all

gdb-server: $(ISO)
	qemu-system-i386 -display gtk -cdrom $(ISO) -S -s
	# gdb build/kfs.elf
	# target remote localhost:1234
	# break _start
	# continue
	# info registers esp
	# disassemble main
