lo.mif: rom2mif firmware.bin
	./rom2mif firmware.bin hi.mif mh.mif ml.mif lo.mif
	cp *.mif ../td_proj/al_ip

firmware.elf: firmware.S firmware.c firmware.lds
	riscv64-unknown-elf-gcc -ffreestanding -nostdlib -O -o firmware.elf firmware.S firmware.c \
        -march=rv32imc -mabi=ilp32 --std=gnu99 -Wl,-Bstatic,-T,firmware.lds,-Map,firmware.map,--strip-debug -lgcc

firmware.bin: firmware.elf
	riscv64-unknown-elf-objcopy -O binary firmware.elf firmware.bin

disasm.S: firmware.elf
	riscv64-unknown-elf-objdump -d firmware.elf > disasm.S

firmware.hex: firmware.elf
	riscv64-unknown-elf-objcopy -O verilog firmware.elf firmware.hex

rom2mif: rom2mif.c
	gcc rom2mif.c -o rom2mif

clean:
	rm -f firmware.elf firmware.hex firmware.bin  firmware.map *.mif disasm.S
