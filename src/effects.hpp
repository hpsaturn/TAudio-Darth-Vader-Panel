#define EFFECTDIR "/vader/"

//X(JEDIYET,"not-a-jedi-yet.mp3") \
//X(FORCE, "force.mp3") \

#define EFFECT_LIST \
X(KBREATH, "breathing.mp3") \
X(DKPOWER, "dont-know-power.mp3") \
X(FATHER,  "father.mp3") \
X(FORCEWY, "force-with-you.mp3") \
X(JOINME, "join-me.mp3") \
X(EFCOUNT,"NONE")

#define X(efkey, fname) efkey,
typedef enum EFKEYS : size_t {EFFECT_LIST} EFKEYS;
#undef X

#define X(efkey, fname) fname, 
char const *effects[] = { EFFECT_LIST };
#undef X

