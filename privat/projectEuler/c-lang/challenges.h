#ifndef  CHALLENGES_H
#define CHALLENGES_H

#define LENGTH(x) (sizeof(x) / sizeof(x[0]))
typedef long ANSWER(void);

/* Function signatures the lazy way: */
ANSWER
challenge_1,                                  
  challenge_2,
  challenge_3,
  challenge_4,
  challenge_5,
  challenge_6,
  challenge_7,
  challenge_8,
  challenge_9,
  challenge_10,
  challenge_11,
  challenge_12,
  challenge_13,
  challenge_14,
  challenge_15;

/* Array of challanges */
ANSWER *answers[15] ={ \
  challenge_1,
  challenge_2,
  challenge_3,
  challenge_4,
  challenge_5,
  challenge_6,
  challenge_7,
  challenge_8,
  challenge_9,
  challenge_10,
  challenge_11,
  challenge_12,
  challenge_13,
  challenge_14,
  challenge_15};    

#endif /* CHALLENGES_H */
