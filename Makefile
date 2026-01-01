target_arch = x86_64


build_version = 1.0
build_mode = release
build_name = realms-$(target_arch)-v$(build_version)-$(build_mode)
build_path = build/bin
binary_path = $(build_path)/$(build_name).bin
image_path = $(build_path)/$(build_name).iso
image_path_wsl = $(shell wslpath -w "$(image_path)" | sed 's/\\/\\\\/g')
target_path = $(build_path)/target/$(target_arch)

cxx = /opt/llvm/21.1.8/bin/clang++
ld = /opt/llvm/21.1.8/bin/ld.lld
lld = /opt/llvm/21.1.8/bin/ld.lld
as = nasm
objdump = x86_64-elf-objdump
gdb = gdb
qemu = qemu-system-x86_64.exe

sysroot = /opt/llvm/21.1.8/x86_64-pc-none-elf

cxxflags = \
	-target x86_64-pc-none-elf \
    -isystem /opt/llvm/21.1.8/x86_64-pc-none-elf/include/c++/v1 \
	-O0 \
    -std=c++26 \
    -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare \
    -Waddress-of-packed-member -Wno-unknown-warning-option \
	-Wno-user-defined-literals \
    -fno-omit-frame-pointer \
    -ffreestanding -fno-rtti -fno-exceptions -fno-stack-protector \
	-fno-unwind-tables -fno-asynchronous-unwind-tables \
    -fno-pic \
    -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4.1 -mno-sse4.2 \
    -mno-avx -mno-avx2 -mno-mmx \
    -mcmodel=kernel -mno-red-zone \
	-D__meta_enable_ranges_deduction \
	-D__meta_enable_global_namespace

ldflags = \
	--sysroot=$(sysroot) \
    -L$(sysroot)/lib \
	-L$(sysroot)/lib/baremetal \
	-L/opt/llvm/21.1.8/lib \
	-L/opt/llvm/21.1.8/lib/clang/21/lib/x86_64-unknown-none-elf \
    -static \
	--wrap=malloc --wrap=free --wrap=calloc --wrap=realloc \
    -lc++ -lc++abi -lc -lm -lclang_rt.builtins-$(target_arch) \
	-z max-page-size=0x1000 \
	-v

objects = build/objs/$(target_arch)
includes = libs/ src/ specs/  
src = src/realms/ specs/  

cppsrc := $(shell find $(src) -name *.cpp)
cppsrc += $(shell find src/arch/$(target_arch)/ -name *.cpp)
cppobjs := $(patsubst %.cpp, $(objects)/%.cpp.o, $(cppsrc))
asmsrc := $(shell find src/arch/$(target_arch)/ -name *.s)
asmobjs := $(patsubst %.s, $(objects)/%.s.o, $(asmsrc))

$(cppobjs): $(objects)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)  
	@printf "%-10s %-30s\n" "cxx" "$<"  
	@$(cxx) $(cxxflags) $(patsubst %/,-I%/,$(includes)) -c $< -o $@
	@$(objdump) -D -S -C $@ > $(patsubst %.cpp.o,%.dec,$@)

$(asmobjs): $(objects)/%.s.o: %.s
	@mkdir -p $(dir $@)
	@printf "%-10s %-30s\n" "as" "$<"
	@$(as) -f elf64 $< -o $@

kernel-x86_64: $(asmobjs) $(cppobjs)
	@echo "building x86_64 kernel..."
	@mkdir -p $(build_path)
	@mkdir -p $(target_path)
	@mkdir -p $(target_path)/EFI/BOOT
	@mkdir -p $(target_path)/limine
	@echo "linking object files..."
	@$(ld) -o $(binary_path) \
		-T build/target/linkscript-x86_64.ld \
		$(asmobjs) $(cppobjs) $(ldflags)
# 	@cp -v libs/limine/limine-bios.sys \
# 		libs/limine/limine-bios-cd.bin \
# 		libs/limine/limine-uefi-cd.bin \
# 		$(target_path)/limine
# 	@cp -v build/target/$(target_arch)/limine.conf \
# 		$(target_path)/limine
# 	@cp -v libs/limine/BOOTX64.EFI \
# 		libs/limine/BOOTIA32.EFI \
# 		$(target_path)/EFI/BOOT
	@cp -v $(binary_path) \
		$(target_path)
# 	@xorriso -as mkisofs -R -r -J -b limine/limine-bios-cd.bin \
# 		-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
# 		-apm-block-size 2048 --efi-boot limine/limine-uefi-cd.bin \
# 		-efi-boot-part --efi-boot-image --protective-msdos-label \
# 		$(target_path) -o $(image_path)
# 	@limine bios-install $(image_path)
	@mkdir -p $(target_path)/boot/grub/
	@cp -v build/target/$(target_arch)/grub.cfg \
		$(target_path)/boot/grub
	@x86_64-elf-grub-mkrescue -o $(image_path) $(target_path)

run-x86_64: kernel-x86_64
	@echo "running kernel $(target_arch), with $(qemu)..."
	@$(qemu) -cdrom $(image_path_wsl) \
		-serial stdio \
		-cpu qemu64,+ssse3,+sse4.1,+sse4.2 \
		-smp 4 -m 128m

clean:
	@echo "cleaning files..."
	@rm -rf build/objs
	@rm -rf build/bin