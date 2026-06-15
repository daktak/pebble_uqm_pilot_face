#pragma once

#define RACE_COUNT 25

typedef enum {
  SPATHI = 0,
  ANDROSYNTH,
  ARILOU,
  CHENJESU,
  CHMMR,
  DRUUGE,
  HUMAN,
  ILWRATH,
  KOHRAH,
  MELNORME,
  MMRNHRM,
  MYCON,
  ORZ,
  PKUNK,
  SHOFIXTI,
  SLYLANDRO,
  SUPOX,
  SYREEN,
  THRADDASH,
  UMGAH,
  URQUAN,
  UTWIG,
  VUX,
  YEHAT,
  ZOQFOTPIK,
} RaceID;

static const char *RACE_NAMES[RACE_COUNT] = {
  "Spathi", "Androsynth", "Arilou", "Chenjesu", "Chmmr",
  "Druuge", "Human", "Ilwrath", "Kohr-Ah", "Melnorme",
  "Mmrnmhrm", "Mycon", "Orz", "Pkunk", "Shofixti",
  "Slylandro", "Supox", "Syreen", "Thraddash", "Umgah",
  "Ur-Quan", "Utwig", "Vux", "Yehat", "Zoq-Fot-Pik",
};
