/*

  Q-dex interface functios definitions
  =======================================
*/

#ifndef __Q_DEX_H_
#define __Q_DEX_H_


//#pragma pack(1)

/* parameters string in calls to quick indx */
struct q_parm_
{
	   unsigned char indx_num;
	   unsigned char reserved;
	   unsigned short int option;  
	   unsigned short int hi_word_write_part_offset;
	   unsigned short int write_part_offset; /* offset of string in partial write */
	   unsigned short int write_part_length; /* length of string in partial write */
	   char filler[20];
};

//#pragma pack()

/* definitions of return codes */
#define OK							0
#define Q_ERROR						0x80
#define ERR_NOT_FOUND				1
#define ERR_INDX_START				2
#define ERR_INDX_READD				3
#define ERR_DELETED					4
#define ERR_EXISTS					5
#define ERR_DISK_READ				6
#define ERR_DISK_WRITE				7
#define ERR_INDX_NOT_LOADED			8
#define ERR_INDX_WRITE				9
#define ERR_INDX_DISK_MATCH			0xA
#define ERR_FILE_NOT_OPENED			0xB
#define ERR_LOAD_FAIL				0xC
#define ERR_BAD_FUNCTION_SECTION	0x20
#define ERR_FILE_FULL				0x21
#define ERR_RECORD_OVERFLOW			0x22
#define ERR_EXP_NOT_FOUND			0x23
#define ERR_EXP 					0x24
#define ERR_EXP_FILE_FULL			0x25
#define ERR_EXP_DELETED				0x26
#define ERR_EXP_EXISTS				0x27
#define ERR_SUP_IDX					0x28
#define ERR_NOT_OPENED				0x29
#define ERR_SEEK					0x2A
#define ERR_MAP 					0xFD
#define ERR_DRVPOS					0xFE
#define BAD_FUNCTION_NO				0xFF
#define Q_DEX_BUSY					0x100


#ifdef __cplusplus
extern "C" {
#endif


short int APIENTRY q_chk ();
short int APIENTRY q_dismount (struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_set_parm (struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_get_parm (struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_write_indx(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_write_direct_indx(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_write_next_indx(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_get_key_data(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_read(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_start(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_readn(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_readp(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_read_last(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_insert(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_write(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_write_part(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_del(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_undel(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_getactivestat(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_open(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_close(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_flush(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_get_block(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_set_block(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_file_chksum(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_active_keys_num(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_sread(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_swrite(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_rread(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_rwrite(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_fread(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_fwrite(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_seek(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_tell(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_fview(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_empty(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_mask_chksum(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_set_sector(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_free_blocks(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_free_abs_block(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_set_active_keys(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_get_sector(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_add_part(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_block_fwrite(struct q_parm_ * q_parm_p, void *buff);
short int APIENTRY q_first_byte_chksum(struct q_parm_ * q_parm_p, void *buff);

#ifdef __cplusplus
}
#endif


#endif //Q_DEX_H