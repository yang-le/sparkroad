SECTIONS {
	.memory : {
		. = 0x0000000;
		*(.init);
		*(.text);
		*(*);
		. = ALIGN(4);
		end = .;
	}
}

