// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
#ifndef MAME_MACHINE_NAOMIGD_H
#define MAME_MACHINE_NAOMIGD_H

#pragma once

#include "machine/naomibd.h"
#include "cpu/pic16c62x/pic16c62x.h"


class naomi_gdrom_board : public naomi_board
{
public:
	template <typename T, typename U>
	naomi_gdrom_board(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, T &&eeprom_tag, const char *_image_tag, U &&picregion_tag)
		: naomi_gdrom_board(mconfig, tag, owner, clock)
	{
		eeprom.set_tag(std::forward<T>(eeprom_tag));
		set_image_tag(_image_tag);
		picdata.set_tag(std::forward<U>(picregion_tag));
	}

	template <typename T>
	naomi_gdrom_board(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, const char *_image_tag, T &&picregion_tag)
		: naomi_gdrom_board(mconfig, tag, owner, clock)
	{
		picdata.set_tag(std::forward<T>(picregion_tag));
		set_image_tag(_image_tag);
	}

	naomi_gdrom_board(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void device_add_mconfig(machine_config &config) override;
	void sh4_map(address_map &map);
	void sh4_io_map(address_map &map);
	void pic_map(address_map &map);

	void set_image_tag(const char *_image_tag)
	{
		image_tag = _image_tag;
	}

	uint8_t *memory(uint32_t &size) { size = dimm_data_size; return dimm_data; }

	virtual const tiny_rom_entry *device_rom_region() const override;

	DECLARE_READ64_MEMBER(i2cmem_dimm_r);
	DECLARE_WRITE64_MEMBER(i2cmem_dimm_w);
	DECLARE_READ8_MEMBER(pic_dimm_r);
	DECLARE_WRITE8_MEMBER(pic_dimm_w);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

	virtual void board_setup_address(uint32_t address, bool is_dma) override;
	virtual void board_get_buffer(uint8_t *&base, uint32_t &limit) override;
	virtual void board_advance(uint32_t size) override;

private:
	enum { FILENAME_LENGTH=24 };

	required_device<sh4_device> m_maincpu;
	required_device<pic16c621a_device> m_securitycpu;

	const char *image_tag;
	optional_region_ptr<uint8_t> picdata;

	uint32_t dimm_cur_address;
	uint8_t picbus;
	uint8_t picbus_pullup;
	uint8_t picbus_io[2]; // 0 for sh4, 1 for pic
	bool picbus_used;

	// Note: voluntarily not saved into the state
	uint8_t *dimm_data;
	uint32_t dimm_data_size;

	static const uint32_t DES_LEFTSWAP[];
	static const uint32_t DES_RIGHTSWAP[];
	static const uint32_t DES_SBOX1[];
	static const uint32_t DES_SBOX2[];
	static const uint32_t DES_SBOX3[];
	static const uint32_t DES_SBOX4[];
	static const uint32_t DES_SBOX5[];
	static const uint32_t DES_SBOX6[];
	static const uint32_t DES_SBOX7[];
	static const uint32_t DES_SBOX8[];
	static const uint32_t DES_MASK_TABLE[];
	static const uint8_t DES_ROTATE_TABLE[16];

	void find_file(const char *name, const uint8_t *dir_sector, uint32_t &file_start, uint32_t &file_size);

	inline void permutate(uint32_t &a, uint32_t &b, uint32_t m, int shift);
	void des_generate_subkeys(const uint64_t key, uint32_t *subkeys);
	uint64_t des_encrypt_decrypt(bool decrypt, uint64_t src, const uint32_t *des_subkeys);
	uint64_t rev64(uint64_t src);
	uint64_t read_to_qword(const uint8_t *region);
	void write_from_qword(uint8_t *region, uint64_t qword);
};

DECLARE_DEVICE_TYPE(NAOMI_GDROM_BOARD, naomi_gdrom_board)

#endif // MAME_MACHINE_NAOMIGD_H
