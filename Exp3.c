#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

typedef struct covStore {
    size_t mySize;
    uint32_t* myVals;
    bool** covMat;
} covStore;

covStore* initCov(covStore* this, const uint32_t* vals, size_t sz);
void genCover(covStore* this);
void freeCov(covStore* this);
void printCov(const covStore* this);


int main(int argc, char** argv) {
    if (argc < 2)
        return printf("Usage: %s NUMBERS", argv[0]);

    uint32_t* vals = malloc((argc - 1) * sizeof(uint32_t));
    for (int i = 1; i < argc; i++)
        vals[i - 1] = (uint32_t)atoll(argv[i]);
    covStore store;
    if (!initCov(&store, vals, argc - 1))
        return 1;
    free(vals);
    genCover(&store);
    printCov(&store);
    freeCov(&store);
    return 0;
}

int __c__(const uint32_t* lhs, const uint32_t* rhs) {return *lhs - *rhs;}

uint32_t gcdUi32(uint32_t lhs, uint32_t rhs) {
    if (lhs > rhs)
        lhs ^= rhs, rhs ^= lhs, lhs ^= rhs;
    while (lhs) {
        uint32_t rem = rhs % lhs;
        rhs = lhs;
        lhs = rem;
    }
    return rhs;
}

covStore* initCov(covStore* this, const uint32_t* vals, size_t sz) {
    this->myVals = malloc(sz * sizeof(uint32_t));
    this->mySize = sz;
    for (size_t i = 0; i < sz; i++)
        this->myVals[i] = vals[i];
    qsort(this->myVals, this->mySize, sizeof(unsigned), 
        (int(*)(const void*, const void*))__c__);
    if (this->myVals[0] == 0)
        return NULL;
    
    // this->isGrid = UNCONFIRMED;
    this->covMat = malloc(this->mySize * sizeof(bool*));
    for (size_t row = 0; row < this->mySize - 1; row++)
        this->covMat[row] = calloc(this->mySize - 1 - row, sizeof(bool));
    return this;
}

void freeCov(covStore* this) {
    free(this->myVals);
    for (size_t i = 0; i < this->mySize - 1; i++)
        free(this->covMat[i]);
    free(this->covMat);
}

bool valInCovMat(const covStore* this, size_t row, size_t col) {
    if (row == col || row >= this->mySize || col >= this->mySize)
        return false;
    if (row < col)
        return this->covMat[row][col - row - 1];
    else return this->covMat[col][row - col - 1];
}

bool* ptrInCovMat(covStore* this, size_t row, size_t col) {
    if (row == col || row >= this->mySize || col >= this->mySize)
        return NULL;
    if (row < col)
        return this->covMat[row] + (col - row - 1);
    else return this->covMat[col] + (row - col - 1);
}

uint64_t isGrid(const covStore* this) {
    for (size_t row = 0; row < this->mySize; row++) {
        for (size_t col = row + 1; col < this->mySize; col++) {
            const uint32_t gcd = gcdUi32(this->myVals[row], this->myVals[col]),
                lcm = this->myVals[row] * this->myVals[col] / gcd;
            const uint32_t maxElem = this->myVals[this->mySize - 1],
                minElem = this->myVals[0];
            uint64_t _; uint32_t* res = (uint32_t*)&_;
            if (maxElem % lcm != 0) {
                res[0] = maxElem;
                res[1] = maxElem % this->myVals[row] == 0 ? 
                    this->myVals[col] : this->myVals[row];
                return _; 
            }
            if (gcd % this->myVals[0] != 0) {
                res[0] = minElem;
                res[1] = this->myVals[row] % minElem == 0 ?
                    this->myVals[col] : this->myVals[row];
                return _; 
            }
        }
    }
    return ~0;
}

uint32_t isComplimentary(const covStore* this) {
    uint32_t* compliments = malloc(this->mySize * sizeof(uint32_t));
    uint32_t self = this->myVals[this->mySize - 1];
    for (size_t i = 0; i < this->mySize; i++) {
        for (size_t j = 0; j < this->mySize; j++)
            if (this->myVals[i] * this->myVals[j] == self 
                    && gcdUi32(this->myVals[i], this->myVals[j]) == 1) {
                compliments[i] = this->myVals[j];
                compliments[j] = this->myVals[i];
            }
    }
    for (size_t i = 0; i < this->mySize; i++)
        if (compliments[i] == 0) {
            free(compliments);
            return this->myVals[i];
        }
    free(compliments);
    return ~0U;
}

void printCov(const covStore* this) {
    puts("Covered Relation");
    size_t printed = 0;
    for (size_t r = 0; r < this->mySize; r++)
        for (size_t c = r + 1; c < this->mySize; c++) 
            if (valInCovMat(this, r, c)) {
                printf("<%u, %u> ", this->myVals[r], this->myVals[c]);
                if ((++printed & 0b111) == 0)
                    putchar('\n');
            }
    putchar('\n');
    uint64_t gridRes = isGrid(this);
    if (gridRes == ~0) {
        puts("The division relation is a grid.");
        uint32_t cmplRes = isComplimentary(this);
        if (cmplRes == ~0)
            puts("The relation is complimentary");
        else printf("The set is not complimentary:\n"
            "Element %u has no compliment.\n", cmplRes);
    } else 
    printf("The division relation is not a grid.\nTuple <%u, %u> is unbounded\n",
        ((uint32_t*)&gridRes)[0], ((uint32_t*)&gridRes)[1]);
}

void genCover(covStore* this) {
    for (size_t row = 0; row < this->mySize; row++) {
        for (size_t col = row + 1; col < this->mySize; col++)
            if (this->myVals[col] % this->myVals[row] == 0)
                *ptrInCovMat(this, row, col) = true;
    }

    for (size_t row = 0; row < this->mySize; row++) {
        for (size_t col = row + 1; col < this->mySize; col++) {
            if (valInCovMat(this, row, col))
                for (size_t i = col + 1; i < this->mySize; i++)
                    if (valInCovMat(this, col, i))
                        *ptrInCovMat(this, row, i) = false;
        }
    }
}

