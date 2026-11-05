.PHONY: all run build clean fclean re check-multiboot

BUILD_DIR = build
BOOT_DIR = $(BUILD_DIR)/boot
ISO_DIR = $(BUILD_DIR)/iso
GRUB_DIR = $(ISO_DIR)/boot/grub

ELF = $(BUILD_DIR)/kfs.elf
ISO = $(ISO_DIR)/kfs.iso

LINKER_SCRIPT = link/linker.ld

C_SRC = src/kfs/main.c
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

run:
	qemu-system-i386 -display gtk -cdrom $(ISO)

build: $(ELF)

$(ISO): $(ELF)
	mkdir -p $(GRUB_DIR)
	cp grub/grub.cfg $(GRUB_DIR)
	cp $(ELF) $(ISO_DIR)/boot/kfs.elf
	grub-mkrescue $(ISO_DIR) -o $(ISO)

$(ELF): $(OBJ) $(LINKER_SCRIPT)
	ld -m elf_i386 $(OBJ) -T link/linker.ld -o $@

$(BUILD_DIR)/%.o: src/%.s
	mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(BUILD_DIR)/%.o: src/%.c
	mkdir -p $(dir $@)
	cc $(CFLAGS) -I include -c $< -o $@

check-multiboot:
	grub-file --is-x86-multiboot2 src/kfs.elf

clean:
	rm -rf build/{boot,kfs,kfs.elf,iso}

fclean: clean
	rm -rf build

re: fclean all
