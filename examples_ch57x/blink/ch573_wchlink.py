#!/usr/bin/env python
import sys
from time import sleep
import argparse
from array import array

import usb.core
import usb.util

CHIPTYPE_CH573 = 0x02

CH_USB_VENDOR_ID   = 0x1a86    # VID
CH_USB_PRODUCT_ID  = 0x8010    # PID
CH_USB_EP_OUT      = 0x01      # endpoint for command transfer out
CH_USB_EP_OUT_DATA = 0x02      # endpoint for data transfer out
CH_USB_EP_IN       = 0x81      # endpoint for reply transfer in
CH_USB_EP_IN_DATA  = 0x82      # endpoint for data reply transfer in
CH_USB_PACKET_SIZE = 256       # packet size
CH_USB_TIMEOUT     = 5000      # timeout for USB operations

CH_STR_PROG_DETECT = (0x81, 0x0d, 0x01, 0x01)
CH_STR_PROG_SPEED  = (0x81, 0x0c, 0x02, 0x01, 0x02) # (0x01: 6000kHz, 0x02: 4000kHz, 0x03: 400kHz)
CH_STR_CHIP_DETECT = (0x81, 0x0d, 0x01, 0x02)
CH_STR_CHIP_SPEED  = (0x81, 0x0c, 0x02, CHIPTYPE_CH573, 0x02)
CH_STR_FLASH_PREP  = (0x81, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00) # send to addr 0x00000000
CH_STR_POLL_DEBUG  = (0x81, 0x08, 0x06, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01)
CH_STR_ACK1_DEBUG  = (0x81, 0x08, 0x06, 0x05, 0x00, 0x00, 0x00, 0x00, 0x02)
CH_STR_ACK2_DEBUG  = (0x81, 0x08, 0x06, 0x04, 0x00, 0x00, 0x00, 0x04, 0x02)

device = usb.core.find(idVendor=CH_USB_VENDOR_ID, idProduct=CH_USB_PRODUCT_ID)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--flash', help='Flash .bin file. If no file is provided a blinky will be flashed')
    parser.add_argument('--dump', help='Dump memory region, use with --length to batch n bytes')
    parser.add_argument('--length', help='Number of bytes to dump')
    parser.add_argument('--terminal', help='Open debug interface terminal', action='store_true')
    parser.add_argument('--toggle-3v', help='Toggle the 3v3 line, to turn the chip off and on again', action='store_true')
    parser.add_argument('--toggle-5v', help='Toggle the 5v line, to turn the chip off and on again', action='store_true')
    parser.add_argument('--reset', help='Reset', action='store_true')
    args = parser.parse_args()

    if device is None:
        print("no programmer found")
        exit(0)

    # Get an endpoint instance
    cfg = device.get_active_configuration()
    intf = cfg[(0, 0)]

    # Claim the interface
    usb.util.claim_interface(device, intf)

    prog_init()

    if args.toggle_3v or args.toggle_5v:
        toggle_power(args.toggle_3v, args.toggle_5v)

    chip_init()

    if args.flash:
        flash(args.flash)
        reset()
        if args.terminal:
            open_terminal()
    elif args.reset:
        reset()
        if args.terminal:
            open_terminal()
    elif args.terminal:
        open_terminal()
    elif args.dump:
        dump(args.dump, args.length)
    elif not (args.toggle_3v or args.toggle_5v):
        flash()
        reset()

    # Release the interface when done
    usb.util.release_interface(device, intf)
    print('done')

flashloader = bytes.fromhex(
        '797122d44ad052cc06d626d24ece56ca5ac85ec6937715002a842e8a32899def'
        '9377240099cbb786070001468145054569229377f50f09459deb9377440091cb'
        '85660146d28505458d2a9377f50f114599ef937784019de7014511a881460146'
        '81452145992a9377f50f0545d5dbb250225492540259f249624ad24a424bb24b'
        '45610290b75400201309f90f93840410937b840013598900814a330a9a404188'
        '330b9a0063900b0221c0938904f0930600104e86da850d4509221375f50f19cd'
        '414575b793060010138604f0da850945ed201375f50f69d9214551bf83a70900'
        '9109be9ae39c99fe7d1993840410e31909fa3dd0b76700209c4be38f57f5c9b7'
        '2303048095472303f4802302a480828083076480e3ce07fe2303048082808307'
        '6480e3ce07fe03454480828083076480e3ce07fe2302a4808280411126c44ac2'
        '4ec006c61377f50bad47aa892e8995446306f70019456d37653f8d444e854d37'
        'fd59fd1463983401b240a2441249824941018280135509011375f50f453f2209'
        'cdb7011126cc06ceb704080051371545853f713769372ac6a53f324593771500'
        '89eb136515001375f50ff240e24405618280fd14e9fc0145cdbf397126dc4ada'
        '4ed852d656d45ad25ed006de62ce66ccb7e700e07d5783aa070022c603aa4700'
        '23a0e71823a2e718b7170040130770052380e704130780fa2380e70483c74704'
        '130975ffb684e2071379f90f8546aa89ae8b328b37240040e187015763fa2601'
        '6308d5008946130700026313d5000157d98f93f7f70fb71c00402382fc041147'
        '2303e4801305f00f653d094cd13563692c11b7070700be9b37870700795563fe'
        'eb02b3879b00636af702a9476399f90689e48144713d26850da0de850945753d'
        '050b0345fbfffd14850b4d3581c493f7fb0ffdf7fd3569fd7d55b71700401307'
        '70052380e704130780fa2380e70403c74704418b2382e704f250b7e700e023a0'
        '571123a24711e25432445259c259325aa25a125b825b724ce24c21618280a547'
        '6395f90685691309f00fb3069900b3f42b01b6941349f9ffb3749900416b3379'
        '7901856b9387f9ffb3f7270199e363fc340193d94900c147e3e637ff99bf0569'
        'c1697d19d9b71305800d63886901130500026384790113051008ca85fd3b9135'
        '21dd4e99b3843441d9b7de852d45f533da94e3009bf2050bd933a30fabfed5bf'
        '9387f9ff93f7f70f636cfc0803c75c04b70708001377070219e3b78707007955'
        'e3fdfbf033879b00e3e9e7f089476390f90489805549e38e04ecde850945713b'
        '110b0327cbff91472320e48003076480e34e07fe23032481fd17edfbfd14910b'
        '81c493f7fb0fe9ff6d3b71f5f1b58547e387f9f4de852d458d331389f4ffe38a'
        '04e8353b9377390091eb8326048003270b0093074b00e39fe6e63e8bca84f1bf'
        'a1476392f904b53b8144638d0b008d479304c0036388fb009304000563848b01'
        '930440041375c507e30595e41945c939c5310545f1312685d5390945c5399133'
        'e31905e291bde38609e2f15425b5'
        )

blink_bin = bytes.fromhex(
        '6f00000c00000000000000006f0080086f006008000000000000000000000000'
        '00000000000000000000000000000000000000006f004006000000006f00e005'
        '000000006f0080056f0060056f0040056f0020056f0000056f00e0046f00c004'
        '000000006f0060046f0040046f0020046f0000046f00e0036f00c00300000000'
        '6f0060036f0040036f0020036f0000036f00e00201a001a001a001a001a001a0'
        '01a001a001a001a001a001a001a001a001a001a001a001a001a001a001a001a0'
        '974100209381017417810020130181f313054000973500209385c57217360020'
        '1306067e63fac5008322050023a0550011059105e3eac5fe1305802f97450020'
        '938545f0174600201306c6ef63fac5008322050023a0550011059105e3eac5fe'
        '17450020130505ee97450020938585ed6377b500232005001105e36db5fe9302'
        '800873a00230973200209382a26b93e212007390523097020000938282137390'
        '123473002030b717004093870704130770052380e700130780fa2380e7000100'
        '0100371700408346b70493f6f60da305d70437071400b71600401307870498c6'
        '23800700010001000100010001000100b727004013073005a383e78082809307'
        '80253305f502814763c3a7008280010001008507d5bf411126c24ac0b7140040'
        '3719004022c406c61304f5ff9384c40a1309890a63580400b240224492440249'
        '410182809c401305100293e707109cc07d378327090093e707102320f90001c4'
        '13051002693f7d14f1b7b70700e09387073898431377070c6dff220513655508'
        '88c38280011122cc26ca4ac84ec606ceaa892e89b2840144634d240195c03545'
        'e9376244f240d2444249b249294505616dbfb38789000385070005047d37e9bf'
        'f2406244d2444249b24905618280411106c6d13db71700409387470b98431545'
        '1377f7ef98c3b71700409387070a98431367071098c3b71700409387870a9843'
        '1367071098c3b7f700e01307701098c319370546a54517050000130565019d37'
        '1305b010e93d0545fd35ddbf7e206368353733207e000000'
    )

def wch_link_command(cmd):
    device.write(CH_USB_EP_OUT, cmd)
    return list( device.read(CH_USB_EP_IN, CH_USB_PACKET_SIZE, CH_USB_TIMEOUT) )

def wch_link_send_data(data):
    padding_len = CH_USB_PACKET_SIZE - (len(data) % CH_USB_PACKET_SIZE)
    data += bytes([0xff] * padding_len)
    for b in range(0, len(data), CH_USB_PACKET_SIZE):
        device.write(CH_USB_EP_OUT_DATA, data[b:b +CH_USB_PACKET_SIZE])

def toggle_power(do_3v, do_5v):
    if do_3v:
        assert wch_link_command((0x81, 0x0d, 0x01, 0x0a)) == [0x82, 0x0d, 0x01, 0x0a]
        sleep(0.1)
        assert wch_link_command((0x81, 0x0d, 0x01, 0x09)) == [0x82, 0x0d, 0x01, 0x09]
    if do_5v:
        assert wch_link_command((0x81, 0x0d, 0x01, 0x0c)) == [0x82, 0x0d, 0x01, 0x0c]
        sleep(0.1)
        assert wch_link_command((0x81, 0x0d, 0x01, 0x0b)) == [0x82, 0x0d, 0x01, 0x0b]

def prog_init():
    prog_info = wch_link_command(CH_STR_PROG_DETECT)
    # print( [hex(x) for x in prog_info] )
    if prog_info[5] == 18:
        print(f'* linkE v{prog_info[3]}.{prog_info[4]} found')
    assert wch_link_command(CH_STR_PROG_SPEED) == [0x82, 0x0c, 0x01, 0x01]

def chip_init():
    assert wch_link_command(CH_STR_CHIP_DETECT)[3] == CHIPTYPE_CH573
    print('* ch573 found, set speed to 4000kHz') # might be ok on 6000kHz too
    assert wch_link_command(CH_STR_CHIP_SPEED) == [0x82, 0x0c, 0x01, 0x01]

def flash(fw = None):
    fw_bin = blink_bin
    if fw:
        fw_bin = open(fw, 'rb').read()
        print(f'* flashing {fw} (len={len(fw_bin)})')
    else:
        print('* flashing blinky example')
    assert wch_link_command(CH_STR_FLASH_PREP + tuple(len(fw_bin).to_bytes(4))) == [0x82, 0x01, 0x01, 0x01]

    assert wch_link_command((0x81, 0x02, 0x01, 0x05)) == [0x82, 0x02, 0x01, 0x05] # what's this?

    wch_link_send_data(flashloader)

    assert wch_link_command((0x81, 0x02, 0x01, 0x07)) == [0x82, 0x02, 0x01, 0x07] # what's this?
    assert wch_link_command((0x81, 0x02, 0x01, 0x02)) == [0x82, 0x02, 0x01, 0x02] # what's this?

    wch_link_send_data(fw_bin)

    assert wch_link_command((0x81, 0x02, 0x01, 0x08)) == [0x82, 0x02, 0x01, 0x08] # what's this?

def dump(address, length):
    address = int(address, 16)
    if not length:
        length = 4
    elif length[:2] == '0x':
        length = int(length, 16)
    else:
        length = int(length)
    length += (4 - (length % 4)) if length % 4 else 0
    address -= address % 4

    cmd = [0x81, 0x03, 0x08] + list(address.to_bytes(4)) + list(length.to_bytes(4))
    wch_link_command(cmd)
    assert wch_link_command((0x81, 0x02, 0x01, 0x0c)) == [0x82, 0x02, 0x01, 0x0c] # what's this?
    res = array('I', bytes( device.read(CH_USB_EP_IN_DATA, CH_USB_PACKET_SIZE, CH_USB_TIMEOUT) ))
    res.byteswap()
    print(f'{address:08x}: {[hex(x) for x in res.tobytes()]}'.replace("'",""))

def open_terminal():
    assert wch_link_command((0x81, 0x08, 0x06, 0x10, 0x80, 0x00, 0x00, 0x01, 0x02)) == [0x82, 0x08, 0x06, 0x10, 0x80, 0x00, 0x00, 0x01, 0x00]
    assert wch_link_command((0x81, 0x08, 0x06, 0x10, 0x80, 0x00, 0x00, 0x03, 0x02)) == [0x82, 0x08, 0x06, 0x10, 0x80, 0x00, 0x00, 0x03, 0x00]

    assert wch_link_command((0x81, 0x08, 0x06, 0x10, 0x80, 0x00, 0x00, 0x01, 0x02)) == [0x82, 0x08, 0x06, 0x10, 0x80, 0x00, 0x00, 0x01, 0x00]
    assert wch_link_command((0x81, 0x08, 0x06, 0x16, 0x00, 0x00, 0x07, 0x00, 0x02)) == [0x82, 0x08, 0x06, 0x16, 0x00, 0x00, 0x07, 0x00, 0x00]

    assert wch_link_command((0x81, 0x08, 0x06, 0x10, 0x40, 0x00, 0x00, 0x01, 0x02)) == [0x82, 0x08, 0x06, 0x10, 0x40, 0x00, 0x00, 0x01, 0x00]

    try:
        while True:
            res = wch_link_command(CH_STR_POLL_DEBUG)
            if res[7] == 0x85:
                print(chr(res[6]), end='')
                assert wch_link_command(CH_STR_ACK1_DEBUG) == [0x82, 0x08, 0x06, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00]
                assert wch_link_command(CH_STR_ACK2_DEBUG) == [0x82, 0x08, 0x06, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00]
    except KeyboardInterrupt:
        print()

def reset():
    assert wch_link_command((0x81, 0x0b, 0x01, 0x01)) == [0x82, 0x0b, 0x01, 0x01]
    assert wch_link_command((0x81, 0x0d, 0x01, 0xff)) == [0x82, 0x0d, 0x01, 0xff]

if __name__ == '__main__':
    main()
