#pragma once
#include <array>
#include <vector>
#include <random>
#include <string>

#include "Constants.h"

namespace Chess
{
    class MagicRooks
    {
    public:

        static inline Magic rookMagicTable[64];

        static uint64_t generateMask(int square) {
            uint64_t mask = 0ULL;
            int rank = square / 8;
            int file = square % 8;

            // Generate rays in all four orthogonal directions
            // Exclude edge squares

            // North
            for (int r = rank + 1; r < 7; r++)
                mask |= (1ULL << (r * 8 + file));

            // South
            for (int r = rank - 1; r > 0; r--)
                mask |= (1ULL << (r * 8 + file));

            // East
            for (int f = file + 1; f < 7; f++)
                mask |= (1ULL << (rank * 8 + f));

            // West
            for (int f = file - 1; f > 0; f--)
                mask |= (1ULL << (rank * 8 + f));

            return mask;
        }

        static uint64_t findMagicNumber(int square, int relevantBits,
            uint64_t mask, std::vector<uint64_t>& attackTable) {
            std::vector<uint64_t> occupancies;
            std::vector<uint64_t> attacks;
            std::vector<uint64_t> usedAttacks;
            int size = 1 << relevantBits;

            // Generate all possible occupancy patterns and their corresponding attacks
            generateOccupancyVariations(mask, occupancies);

            // Generate corresponding attacks for each occupancy
            for (uint64_t occupancy : occupancies) {
                attacks.push_back(generateAttacks(square, occupancy));
            }

            // Try random magic numbers until we find one that works
            std::random_device rd;
            std::mt19937_64 gen(rd());
            std::uniform_int_distribution<uint64_t> dis;

            for (int i = 0; i < 100000000; i++) {  // Maximum attempts
                uint64_t magic = getRandom64(gen);  // Generate random sparse number

                if (popcnt64((mask * magic) & 0xFF00000000000000ULL) < 6)
                    continue;  // Skip if not enough bits in high bytes

                // Reset used attacks array
                usedAttacks.clear();
                usedAttacks.resize(size, 0ULL);

                bool failed = false;

                // Test this magic number against all occupancies
                for (int j = 0; j < occupancies.size(); j++) {
                    uint64_t occupancy = occupancies[j];
                    uint64_t attack = attacks[j];

                    // Calculate magic index
                    uint64_t index = ((occupancy & mask) * magic) >> (64 - relevantBits);

                    if (index >= size) {
                        failed = true;
                        break;
                    }

                    // If this index is empty or contains the same attack pattern
                    if (usedAttacks[index] == 0ULL || usedAttacks[index] == attack) {
                        usedAttacks[index] = attack;
                    }
                    else {
                        // Magic number doesn't work - collision found
                        failed = true;
                        break;
                    }
                }

                if (!failed) {
                    attackTable = usedAttacks;
                    return magic;  // Found a valid magic number
                }
            }

            // If we get here, we failed to find a magic number
            throw std::runtime_error("Failed to find magic number for square " + std::to_string(square));
        }

        static uint64_t getRandom64(std::mt19937_64& gen) {
            std::uniform_int_distribution<uint64_t> dis;
            return dis(gen) & dis(gen) & dis(gen) & dis(gen);
        }

        static void generateOccupancyVariations(uint64_t mask, std::vector<uint64_t>& occupancies) {
            int bits[64];
            int count = 0;
            uint64_t temp = mask;

            while (temp) {
                int bit = std::countr_zero(temp);
                bits[count++] = bit;
                temp &= (temp - 1);
            }

            for (int i = 0; i < (1 << count); i++) {
                uint64_t occupancy = 0ULL;
                for (int j = 0; j < count; j++) {
                    if (i & (1 << j)) {
                        occupancy |= (1ULL << bits[j]);
                    }
                }
                occupancies.push_back(occupancy);
            }
        }

        // Helper function to generate rook attacks for a given square and occupancy
        static uint64_t generateAttacks(int square, uint64_t occupancy) {
            uint64_t attacks = 0ULL;
            int rank = square / 8;
            int file = square % 8;

            // Generate attacks in all four orthogonal directions
            // North
            for (int r = rank + 1; r < 8; r++) {
                uint64_t sq = 1ULL << (r * 8 + file);
                attacks |= sq;
                if (occupancy & sq) break;
            }

            // South
            for (int r = rank - 1; r >= 0; r--) {
                uint64_t sq = 1ULL << (r * 8 + file);
                attacks |= sq;
                if (occupancy & sq) break;
            }

            // East
            for (int f = file + 1; f < 8; f++) {
                uint64_t sq = 1ULL << (rank * 8 + f);
                attacks |= sq;
                if (occupancy & sq) break;
            }

            // West
            for (int f = file - 1; f >= 0; f--) {
                uint64_t sq = 1ULL << (rank * 8 + f);
                attacks |= sq;
                if (occupancy & sq) break;
            }

            return attacks;
        }

        static void debugMagicNumber(int square) {
            uint64_t mask = generateMask(square);
            int relevantBits = std::popcount(mask);
            std::vector<uint64_t> attackTable;

            try {
                uint64_t magic = findMagicNumber(square, relevantBits, mask, attackTable);
                std::cout << "Found magic number for square " << square << ": 0x"
                    << std::hex << magic << std::dec << "\n";

                // Test the magic number with a few random occupancies
                std::vector<uint64_t> occupancies;
                generateOccupancyVariations(mask, occupancies);

                for (int i = 0; i < std::min(5, (int)occupancies.size()); i++) {
                    uint64_t occupancy = occupancies[i];
                    uint64_t index = ((occupancy & mask) * magic) >> (64 - relevantBits);
                    uint64_t attacks = attackTable[index];

                    std::cout << "\nTest case " << i << ":\n";
                    std::cout << "Occupancy:\n";
                    printBitboard(occupancy);
                    std::cout << "Resulting attacks:\n";
                    printBitboard(attacks);
                }
            }
            catch (const std::runtime_error& e) {
                std::cout << "Error: " << e.what() << "\n";
            }
        }

        static void printBitboard(uint64_t bitmap)
        {
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    std::cout << ((bitmap & (1ULL << 63 - i * 8 + j)) == 0 ? "0" : "1") << " ";
                }
                std::cout << std::endl;
            }
        }

        static void initializeMagics() {
            for (int square = 0; square < 64; square++) {
                // Step 1: Generate the mask
                rookMagicTable[square].mask = generateMask(square);

                // Step 2: Calculate the shift
                // Count bits in mask and subtract from 64
                int relevantBits = popcnt64(rookMagicTable[square].mask);
                rookMagicTable[square].shift = 64 - relevantBits;

                // Step 3: Find appropriate magic number and fill the attack table with correcsponding vector
                rookMagicTable[square].magic = findMagicNumber(square, relevantBits,
                    rookMagicTable[square].mask, rookMagicTable[square].attackTable);
            }
        }

        //occupancy here represents all pieces on the board
        //assumes all pieces are enemy, so use & !friendlyPieces to mask them off
        static uint64_t getAttacks(int square, uint64_t occupancy) {
            occupancy &= rookMagicTable[square].mask;
            occupancy *= rookMagicTable[square].magic;
            occupancy >>= rookMagicTable[square].shift;
            return rookMagicTable[square].attackTable[occupancy];
        }
    };

};