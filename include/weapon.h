#ifndef _WEAPON_H_
#define _WEAPON_H_

#define DAMAGE_TYPE_SLASH  0x01
#define DAMAGE_TYPE_THRUST 0x02
#define DAMAGE_TYPE_BLUNT  0x04

#define DAMAGE_TYPE_SHOCK  0x10
#define DAMAGE_TYPE_FIRE   0x20
#define DAMAGE_TYPE_ICE    0x40

// Damage is encoded in the metadata with the following logic
// 0xFF -> Type
// 0xFF00 >> 8 -> Damage (0 - 255)

extern const char* get_damage_type_str(int type);

extern int get_damage_type_from_metadata(int metadata);
extern int get_damage_from_metadata(int metadata);

#endif
