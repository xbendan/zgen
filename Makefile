platform = x86_64
build_version = 1.0
build_mode = release
build_name = zgen-$(platform)-v$(build_version)-$(build_mode)
build_path = build/bin
binary_path = $(build_path)/$(build_name).bin
image_path = $(build_path)/$(build_name).iso
image_path_wsl = $(shell wslpath -w "$(image_path)" | sed 's/\\/\\\\/g')
target_path = $(build_path)/target/$(platform)

cxx = x86_64-elf-g++
ld = x86_64-elf-ld
as = nasm
objdump = x86_64-elf-objdump
qemu = qemu-system-x86_64.exe

cxxflags = \
	-O2 \
	-std=c++26 \
	-Wall -Wextra -Wno-unused-parameter -Wno-sign-compare \
	-Waddress-of-packed-member -Wno-literal-suffix \
	-D__sdk_freestanding__ \
	-ffreestanding -fno-rtti -fno-exceptions -fno-stack-protector \
	-fno-pic \
	-mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4.1 -mno-sse4.2 \
	-mno-avx -mno-avx2 -mno-mmx \
	-mcmodel=kernel -mno-red-zone \
	-g

ldflags = \
	-static \
	-m elf_x86_64 \
	-nostdlib \
	-z max-page-size=0x1000

objects = build/objs/$(platform)
includes = libs/ src/ specs/  
src = src/zgen/ src/misc/ specs/  

cppsrc := $(shell find $(src) -name *.cpp)
cppsrc += $(shell find src/arch/$(platform)/ -name *.cpp)
cppobjs := $(patsubst %.cpp, $(objects)/%.cpp.o, $(cppsrc))
asmsrc := $(shell find src/arch/$(platform)/ -name *.s)
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
	@$(ld) $(ldflags) -o $(binary_path) \
		-T build/target/linkscript-x86_64.ld \
		$(asmobjs) $(cppobjs)
	@cp -v libs/limine/limine-bios.sys \
		libs/limine/limine-bios-cd.bin \
		libs/limine/limine-uefi-cd.bin \
		$(target_path)/limine
	@cp -v build/target/$(platform)/limine.conf \
		$(target_path)/limine
	@cp -v libs/limine/BOOTX64.EFI \
		libs/limine/BOOTIA32.EFI \
		$(target_path)/EFI/BOOT
	@cp -v $(binary_path) \
		$(target_path)
	@xorriso -as mkisofs -R -r -J -b limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
		-apm-block-size 2048 --efi-boot limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		$(target_path) -o $(image_path)
	@limine bios-install $(image_path)

run-x86_64: kernel-x86_64
	@echo "running kernel $(platform), with $(qemu)..."
	@$(qemu) -cdrom $(image_path_wsl) \
		-serial stdio \
		-cpu qemu64,+ssse3,+sse4.1,+sse4.2 \
		-smp 4 -m 128m

clean:
	@echo "cleaning files..."
	@rm -rf build/objs
	@rm -rf build/bin