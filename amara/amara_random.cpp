#pragma once
#ifndef AMARA_RANDOM
#define AMARA_RANDOM

#include "amara.h"

namespace Amara {
    class RNG {
        public:
            std::random_device rd;
            std::default_random_engine e;

            RNG* randomize() {
                e.seed(time(0));
				return this;
            }

			RNG* seed(int s) {
				e.seed(s);
				return this;
			}

            RNG* seed(std::string s, uint32_t st) {
                uint32_t sn; 
                MurmurHash3_x86_32(s.c_str(), s.size(), st, &sn);
				seed(sn);
				return this;
            }

			RNG* seed(std::string s) {
                return seed(s, 0);
			}

            double between(double min, double max) {
                std::uniform_real_distribution<double> dist(0, 1);
                return dist(e);
            }

            double random() {
                return between(0, 1);
            }

            std::string randomString(char min, char max, int length) {
                std::string toReturn = "";
                char c;
                for (int i = 0; i < length; i++) {
                    c = floor(min + random() * (max+1 - min));
                    toReturn += c;
                }
                return toReturn;
            }

            std::string randomString(int length) {
                return randomString('a', 'z', length);
            }
    };
}

#endif
