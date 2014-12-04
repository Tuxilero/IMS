// ╔═════════════════════════════════╗
// ║           Projekt IMS           ║
// ║            Zadani 8             ║
// ║   Model dopravy na dálnici D1   ║
// ╠═════════════════════════════════╣
// ║       xherma25 / xstodu06       ║
// ╚═════════════════════════════════╝


// =========================
// ======= Libraries =======
#include <simlib.h>
#include <time.h>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <iterator>
// ======= Libraries =======
// =========================


// =========================
// ====== Definitions ======
#define DEBUG

#define OUTPUT_FILE             "output.out"
#define SIMULATION_DURATION     1
#define DAY_IN_SECONDS          86400

#define CAR_LENGTH              5.1
#define CAR_SPACING             45
#define CAPACITY                round( (1000 / (CAR_LENGTH + CAR_SPACING)))*2
#define CAPACITY_LIMITED        (CAPACITY / 2) + 5
#define HIGHWAY_LENGTH          204

#define GENERATE_CAR_DAY        0.45       //  6:00 - 18:00 Kolem 150k aut denne
#define GENERATE_CAR_EVENING    0.77        // 18:00 - 22:00
#define GENERATE_CAR_NIGHT      2.31       // 22:00 -  6:00  // Praha-Brno


/*#define GENERATE_CAR_DAY        0.2888        //  6:00 - 18:00 Kolem 180k aut denne
#define GENERATE_CAR_EVENING    0.48        // 18:00 - 22:00
#define GENERATE_CAR_NIGHT      1.45       // 22:00 -  6:00  // Brno-Praha*/


// #define GENERATE_CAR_DAY     800.38      //  6:00 - 18:00
// #define GENERATE_CAR_EVENING 8000.64     // 18:00 - 22:00
// #define GENERATE_CAR_NIGHT       8001.923        // 22:00 -  6:00

#define GENERATE_ACCIDENT       60000

#define PRAHA_BRNO              1
#define BRNO_PRAHA              2

const int EXITS_PRAHA_BRNO[]    = {2, 6, 8, 10, 15, 21, 29, 34, 41, 49, 56, 66, 75, 90, 104, 112/*2x*/, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194/*2x*/, 196/*2x*/, 201, 203};
const int EXITS_BRNO_PRAHA[]    = {0, 2, 7/*2x*/, 9/*2x*/, 13, 21, 25, 35, 41, 50, 57, 62, 69, 84, 91, 99, 113, 122, 137, 147, 154, 162, 169, 174, 182, 188, 191, 195, 198, 201/*2x*/, 202, 203};
const int ENTRIES_PRAHA_BRNO[]  = {0, 1, 2, 6, 8, 12, 15, 21, 29, 34, 41, 49, 56, 66, 81, 90, 104, 112, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194/*2x*/, 196/*2x*/, 201};
const int ENTRIES_BRNO_PRAHA[]  = {0, 2, 7/*2x*/, 9/*2x*/, 13, 21, 25, 35, 41, 50, 57, 62, 69, 84, 99, 113, 128, 137, 147, 154, 162, 169, 174, 182, 188, 193, 195, 197, 201};
// ====== Definitions ======
// =========================


// =========================
// ======== Macros =========
#define isDay(value)            ((long)value%DAY_IN_SECONDS >= 21600 && (long)value%DAY_IN_SECONDS < 64800)
#define isEvening(value)        ((long)value%DAY_IN_SECONDS >= 64800 && (long)value%DAY_IN_SECONDS < 79200)
#define km2ms(value)            value/3.6
#define ms2km(value)            value*3.6

#define isExitPrahaBrno(km)     (std::find(std::begin(EXITS_PRAHA_BRNO), std::end(EXITS_PRAHA_BRNO), km) != std::end(EXITS_PRAHA_BRNO))
#define isExitBrnoPraha(km)     (std::find(std::begin(EXITS_BRNO_PRAHA), std::end(EXITS_BRNO_PRAHA), km) != std::end(EXITS_BRNO_PRAHA))

#define isEntryPrahaBrno(km)    (std::find(std::begin(ENTRIES_PRAHA_BRNO), std::end(ENTRIES_PRAHA_BRNO), km) != std::end(ENTRIES_PRAHA_BRNO))
#define isEntryBrnoPraha(km)    (std::find(std::begin(ENTRIES_BRNO_PRAHA), std::end(ENTRIES_BRNO_PRAHA), km) != std::end(ENTRIES_BRNO_PRAHA))

#define isUnderConstructionPrahaBrno(i) ((i == 38)  || (i >= 40 && i <= 50) || (i >= 65 && i <= 77))
#define isUnderConstructionBrnoPraha(i) ((i >= 127 && i <= 138 ) || (i >= 153  && i <= 163))

#define najezdPrahaBrno(x) \
	((x < 0.1188943582)	?   0 : ((x < 0.3046706608)	?   1 : ((x < 0.3499612859)	?   2 : ((x < 0.36984191)	?   6 : ((x < 0.3852313413)	?   8 : \
	((x < 0.422720046)	?  12 : ((x < 0.4294568351)	?  15 : ((x < 0.4344420591)	?  21 : ((x < 0.4422387698)	?  29 : ((x < 0.4653145182)	?  34 : \
	((x < 0.4820666673)	?  41 : ((x < 0.502142299)	?  49 : ((x < 0.5069478752)	?  56 : ((x < 0.5177267379)	?  66 : ((x < 0.5254336247)	?  81 : \
	((x < 0.5633842036)	?  90 : ((x < 0.5845377215)	? 104 : ((x < 0.590187642)	? 112 : ((x < 0.6062212002)	? 119 : ((x < 0.6234224685)	? 134 : \
	((x < 0.6391416432)	? 141 : ((x < 0.6594687817)	? 146 : ((x < 0.6694482121)	? 153 : ((x < 0.6935030405)	? 162 : ((x < 0.7203154614)	? 168 : \
	((x < 0.743651699)	? 178 : ((x < 0.7516909341)	? 182 : ((x < 0.8762317096)	? 190 : ((x < 0.9647710839)	? 194 : ((x < 0.9878468324)	? 196 : 201))))))))))))))))))))))))))))))

#define sjezdPrahaBrno(x) \
	((isExitPrahaBrno(x)==0)	? 0 : ((x==2)	? 0.33 : ((x==6)	? 0.20 : ((x==8)	? 0.10 : ((x==10)	? 0.0900: \
	((x==15)	? 0.210 : ((x==21)	? 0.292 : ((x==29)	? 0.110 : ((x==34)	? 0.200 : ((x==41)	? 0.150 : \
	((x==49)	? 0.080 : ((x==56)	? 0.135 : ((x==66)	? 0.100 : ((x==75)	? 0.050 : ((x==90)	? 0.120 : \
	((x==104)	? 0.120 : ((x==112)	? 0.100 : ((x==119)	? 0.100 : ((x==134)	? 0.080 : ((x==141)	? 0.100 : \
	((x==146)	? 0.080 : ((x==153)	? 0.100 : ((x==162)	? 0.100 : ((x==168)	? 0.100 : ((x==178)	? 0.100 : \
	((x==182)	? 0.180 : ((x==190)	? 0.180 : ((x==194)	? 0.210 : ((x==196)	? 0.310 : ((x==201)	? 0.360 : 1))))))))))))))))))))))))))))))


#define najezdBrnoPraha(x) \
		((x < 0.09965123588) ?   0 : ((x < 0.1530608107) ?   2 : ((x < 0.2117018472) ?   7 : ((x < 0.2395167995) ?   9 : ((x < 0.2596315672) ?   13 : \
        ((x < 0.2676764604) ?  21 : ((x < 0.2760812702) ?  25 : ((x < 0.2861487925) ?  35 : ((x < 0.2992629316) ?  41 : ((x < 0.3070492933) ?  50 : \
        ((x < 0.3167974532) ?  57 : ((x < 0.3252022629) ?  62 : ((x < 0.3417077276) ?  69 : ((x < 0.3614625788) ?  84 : ((x < 0.3866972849) ?  99 : \
        ((x < 0.4062341586) ?  113 : ((x < 0.4233073788) ? 128 : ((x < 0.4464636941) ? 137 : ((x < 0.455512298) ? 147 : ((x < 0.4752570108) ? 154 : \
        ((x < 0.5140772959) ? 162 : ((x < 0.5368889024) ? 169 : ((x < 0.5616623071) ? 174 : ((x < 0.6145598881) ? 182 : ((x < 0.6783664862) ? 188 : \
        ((x < 0.7420767687) ? 193 : ((x < 0.7859612304) ? 195 : ((x < 0.8714185778) ? 197 : 201))))))))))))))))))))))))))))

#define sjezdBrnoPraha(x) \
	((isExitBrnoPraha(x)==0) ? 0 : ((x==2)   ? 0.1892 : ((x==7)   ? 0.1241 : ((x==9) ? 0.1259 : \
	((x==13)  ? 0.1644 : ((x==21)  ? 0.1866 : ((x==25)  ? 0.2135 : ((x==35)  ? 0.1730 : ((x==41)  ? 0.1222 : \
	((x==50)  ? 0.1206 : ((x==57)  ? 0.1215 : ((x==62)  ? 0.1206 : ((x==69)  ? 0.1208 : ((x==84)  ? 0.1204 : \
	((x==91)  ? 0.1207 : ((x==99)  ? 0.1207 : ((x==113) ? 0.1199 : ((x==122) ? 0.1199 : ((x==137) ? 0.1398 : \
	((x==147) ? 0.1207 : ((x==154) ? 0.1214 : ((x==162) ? 0.1216 : ((x==169) ? 0.1229 : ((x==174) ? 0.1236 : \
	((x==182) ? 0.1240 : ((x==188) ? 0.1372 : ((x==191) ? 0.1420 : ((x==195) ? 0.2100 : ((x==198) ? 0.2 : \
	((x==201) ? 0.18 : ((x==202) ? 0.17 : 1)))))))))))))))))))))))))))))))

#define debugArray(km)  \
	((km == 1)   ? gCounterArray[0]++  : ((km == 4)   ? gCounterArray[1]++  : ((km == 10)  ? gCounterArray[2]++  : ((km == 14)  ? gCounterArray[3]++  : \
	((km == 18)  ? gCounterArray[4]++  : ((km == 25)  ? gCounterArray[5]++  : ((km == 32)  ? gCounterArray[6]++  : ((km == 37)  ? gCounterArray[7]++  : \
	((km == 45)  ? gCounterArray[8]++  : ((km == 53)  ? gCounterArray[9]++  : ((km == 61)  ? gCounterArray[10]++ : ((km == 70)  ? gCounterArray[11]++ : \
	((km == 78)  ? gCounterArray[12]++ : ((km == 85)  ? gCounterArray[13]++ : ((km == 100)  ? gCounterArray[14]++ : ((km == 108) ? gCounterArray[15]++ : \
	((km == 115) ? gCounterArray[16]++ : ((km == 129) ? gCounterArray[17]++ : ((km == 137) ? gCounterArray[18]++ : ((km == 143) ? gCounterArray[19]++ : \
	((km == 150) ? gCounterArray[20]++ : ((km == 157) ? gCounterArray[21]++ : ((km == 165) ? gCounterArray[22]++ : ((km == 173) ? gCounterArray[23]++ : \
	((km == 180) ? gCounterArray[24]++ : ((km == 189) ? gCounterArray[25]++ : ((km == 193) ? gCounterArray[26]++ : ((km == 195) ? gCounterArray[27]++ : \
	((km == 198) ? gCounterArray[28]++ : ((km == 202) ? gCounterArray[29]++ : gCounterArray[30]))))))))))))))))))))))))))))))
// ======== Macros =========
// =========================


// =========================
// ======== Global =========
int gDirection = BRNO_PRAHA;
unsigned long gCounterCar = 0;
unsigned long gCounterAccident = 0;
unsigned long gEnd = 0;
unsigned long gNasrat = 0;

unsigned long gCounterArray[30];
// ======== Global =========
// =========================


// =========================
// ======= Histogram =======
Histogram                   hZdrzeni("Doba cekani v kolone v sec", 0, 900 , 12);
Histogram                   hKilometry("Kolony na km: ", 0, 1, HIGHWAY_LENGTH);
// ======= Histogram =======
// =========================


// =========================
// ======== Classes ========
class HighwayPart : public Store
{
public:
    double mMaxSpeed;
    bool mExit;

    HighwayPart (int capacity, int maxSpeed, bool exit) : Store(capacity)
    {
        mMaxSpeed = km2ms(maxSpeed);
        mExit = exit;
    };
};

HighwayPart *gHighway[HIGHWAY_LENGTH];

class Car : public Process
{

    double mTimeBeforeEntry;
    int mCurrentPosition;
    double mCurrentSpeed;

    void Behavior()
    {
        double randomNumber = Random();

        if (gDirection == PRAHA_BRNO)
            mCurrentPosition = najezdPrahaBrno(randomNumber);
        else{
            mCurrentPosition = najezdBrnoPraha(randomNumber);
			}
		
		if(mCurrentPosition == 201)
			gEnd++;
        mTimeBeforeEntry = Time;
        gHighway[mCurrentPosition]->Enter(this, 1);
		
		if(mCurrentPosition == 201)
			gNasrat++;
        gCounterCar++;
        do
        {
            debugArray(mCurrentPosition);

            if ((Time - mTimeBeforeEntry) > 5)
            {
                hZdrzeni(Time - mTimeBeforeEntry);
                hKilometry(mCurrentPosition);
            }

            // mCurrentSpeed = Random();
            if (ms2km(gHighway[mCurrentPosition]->mMaxSpeed) == 130)
            {
                mCurrentSpeed = Normal(130, 20);
                if (mCurrentSpeed > 130)
                    mCurrentSpeed = 130 - (mCurrentSpeed - 130);
                if (mCurrentSpeed < 80)
                    mCurrentSpeed = 80;

                mCurrentSpeed = km2ms(mCurrentSpeed);
            }
            else
            {
                mCurrentSpeed = gHighway[mCurrentPosition]->mMaxSpeed;
            }
	        Wait(1000 / mCurrentSpeed);
            mTimeBeforeEntry = Time;
            mCurrentPosition++;
            gHighway[mCurrentPosition]->Enter(this, 1);
            gHighway[mCurrentPosition - 1]->Leave(1);

		/*	if(gDirection == PRAHA_BRNO)
				gExit = Random() >= sjezdPrahaBrno(mCurrentPosition);
			else
				gExit = Random() >= sjezdBrnoPraha(mCurrentPosition);*/
			
        }
        while (Random() >= ((gDirection == PRAHA_BRNO) ? sjezdPrahaBrno(mCurrentPosition) : sjezdBrnoPraha(mCurrentPosition)));

        gHighway[mCurrentPosition]->Leave(1);
        Terminate();
    }
};

class Accident : public Process
{
    void Behavior()
    {
        gCounterAccident++;

        int position = Uniform(0, HIGHWAY_LENGTH - 1);
        double speedBackup = gHighway[position]->mMaxSpeed;
        double repairTime;

        Print("\nNehoda na %d km\n", position);
        std::cout << std::endl << "Nehoda na " << position << "km" << std::endl;
        std::cout << "Time: " << Time / 3600 << std::endl;

        int newSpeed = Uniform(10, 80);

        std::cout << "Accident! Speed: " << newSpeed << std::endl;
        repairTime = Uniform(5400, 7200);
        gHighway[position]->mMaxSpeed = km2ms(newSpeed);

        std::cout << "Repair time: " << repairTime << std::endl;
        Wait(repairTime);
        gHighway[position]->mMaxSpeed = speedBackup;
    }
};

class GeneratorAccident : public Event
{
    void Behavior()
    {
        (new Accident)->Activate();
        Activate(Time + Exponential(GENERATE_ACCIDENT));
    }
};


class GeneratorCar : public Event
{
    void Behavior()
    {
        (new Car)->Activate();

        if (isDay(Time))
            Activate(Time + Exponential(GENERATE_CAR_DAY));
        else if (isEvening(Time))
            Activate(Time + Exponential(GENERATE_CAR_EVENING));
        else
            Activate(Time + Exponential(GENERATE_CAR_NIGHT));
    }
};

class GeneratorProgress : public Event
{
    void Behavior()
    {
        if (Time == 0)
            std::cout << "Progress: ";
        std::cout << "䷀" << std::flush;

        Activate(Time + 3600);
    }
};
// ======== Classes ========
// =========================


// =========================
// ======= Functions =======

void initHighway()
{
    HighwayPart *tmp;

    for (int i = 0; i < HIGHWAY_LENGTH; i++)
    {
        bool exit = false;
        int maxSpeed = 130;
        int capacity = CAPACITY;

        if (gDirection == PRAHA_BRNO)
        {
            if (isExitPrahaBrno(i))
                exit = true;

            if (isUnderConstructionPrahaBrno(i))
            {
                maxSpeed = 80;
                capacity = CAPACITY_LIMITED;
            }
        }
        else
        {
            if (isExitBrnoPraha(i))
                exit = true;

            if (isUnderConstructionBrnoPraha(i))
            {
                maxSpeed = 80;
                capacity = CAPACITY_LIMITED;
            }
        }

        tmp = new HighwayPart(capacity, maxSpeed, exit);
        gHighway[i] = tmp;
    }
}

void destroyHighway()
{
    for (int i = 0; i < HIGHWAY_LENGTH; i++)
        delete gHighway[i];
}

// ======= Functions =======
// =========================


// =========================
// ========= MAIN ==========
int main()
{
    SetOutput(OUTPUT_FILE);
    RandomSeed ( time(NULL) );

    gDirection = PRAHA_BRNO;
	//gDirection = BRNO_PRAHA;
    initHighway();
    Init(0, SIMULATION_DURATION * DAY_IN_SECONDS);
    (new GeneratorProgress)->Activate();
    (new GeneratorCar)->Activate();
    (new GeneratorAccident)->Activate(Time + Exponential(GENERATE_ACCIDENT));
    Run();

    destroyHighway();

    Print("Dalnici projelo %d aut\n", gCounterCar);
    Print("Na dalnici se staly %d nehody\n\n", gCounterAccident);

#ifdef DEBUG
	// Praha-Brno
    Print("Na km   0 -   2   (1) projelo %d aut. Mělo projet 34 499\n", gCounterArray[0]);
    Print("Na km   2 -   6   (4) projelo %d aut. Mělo projet 27 776\n", gCounterArray[1]);
    Print("Na km   6 -  12  (10) projelo %d aut. Mělo projet 21 263\n", gCounterArray[2]);
    Print("Na km  12 -  15  (14) projelo %d aut. Mělo projet 22 310\n", gCounterArray[3]);
    Print("Na km  15 -  21  (18) projelo %d aut. Mělo projet 18 494\n", gCounterArray[4]);
    Print("Na km  21 -  29  (25) projelo %d aut. Mělo projet 12 332\n", gCounterArray[5]);
    Print("Na km  29 -  34  (32) projelo %d aut. Mělo projet 12 128\n", gCounterArray[6]);
    Print("Na km  34 -  41  (37) projelo %d aut. Mělo projet 11 769\n", gCounterArray[7]);
    Print("Na km  41 -  49  (45) projelo %d aut. Mělo projet 11 487\n", gCounterArray[8]);
    Print("Na km  49 -  56  (53) projelo %d aut. Mělo projet 13 211\n", gCounterArray[9]);
    Print("Na km  56 -  66  (61) projelo %d aut. Mělo projet 11 372\n", gCounterArray[10]);
    Print("Na km  66 -  75  (70) projelo %d aut. Mělo projet 11 540\n", gCounterArray[11]);
    Print("Na km  75 -  81  (78) projelo %d aut. Mělo projet 11 324\n", gCounterArray[12]);
    Print("Na km  81 -  90  (85) projelo %d aut. Mělo projet 11 958\n", gCounterArray[13]);
    Print("Na km  90 - 104  (97) projelo %d aut. Mělo projet 14 067\n", gCounterArray[14]);
    Print("Na km 104 - 112 (108) projelo %d aut. Mělo projet 14 596\n", gCounterArray[15]);
    Print("Na km 112 - 119 (115) projelo %d aut. Mělo projet 13 884\n", gCounterArray[16]);
    Print("Na km 119 - 134 (127) projelo %d aut. Mělo projet 14 041\n", gCounterArray[17]);
    Print("Na km 134 - 141 (137) projelo %d aut. Mělo projet 14 637\n", gCounterArray[18]);
    Print("Na km 141 - 146 (143) projelo %d aut. Mělo projet 14 868\n", gCounterArray[19]);
    Print("Na km 146 - 153 (150) projelo %d aut. Mělo projet 15 905\n", gCounterArray[20]);
    Print("Na km 153 - 162 (157) projelo %d aut. Mělo projet 15 204\n", gCounterArray[21]);
    Print("Na km 162 - 168 (165) projelo %d aut. Mělo projet 17 149\n", gCounterArray[22]);
    Print("Na km 168 - 178 (173) projelo %d aut. Mělo projet 18 552\n", gCounterArray[23]);
    Print("Na km 178 - 182 (180) projelo %d aut. Mělo projet 18 932\n", gCounterArray[24]);
    Print("Na km 182 - 190 (185) projelo %d aut. Mělo projet 13 712\n", gCounterArray[25]);
    Print("Na km 190 - 194 (192) projelo %d aut. Mělo projet 26 511\n", gCounterArray[26]);
    Print("Na km 194 - 196 (195) projelo %d aut. Mělo projet 30 558\n", gCounterArray[27]);
    Print("Na km 196 - 201 (198) projelo %d aut. Mělo projet 22 272\n", gCounterArray[28]);
    Print("Na km 201 - 203 (202) projelo %d aut. Mělo projet 14 483\n\n", gCounterArray[29]);
#endif

#ifdef DEBUG
	// Brno-Praha
   /* Print("Na km   0 -   2   (1) projelo %d aut. Mělo projet 18 433\n", gCounterArray[0]);
    Print("Na km   2 -   6   (4) projelo %d aut. Mělo projet 24 128\n", gCounterArray[1]);
    Print("Na km   6 -  12  (10) projelo %d aut. Mělo projet 34 458\n", gCounterArray[2]);
    Print("Na km  12 -  15  (14) projelo %d aut. Mělo projet 32 403\n", gCounterArray[3]);
    Print("Na km  15 -  21  (18) projelo %d aut. Mělo projet 31 996\n", gCounterArray[4]);
    Print("Na km  21 -  29  (25) projelo %d aut. Mělo projet 21 349\n", gCounterArray[5]);
    Print("Na km  29 -  34  (32) projelo %d aut. Mělo projet 20 920\n", gCounterArray[6]);
    Print("Na km  34 -  41  (37) projelo %d aut. Mělo projet 20 959\n", gCounterArray[7]);
    Print("Na km  41 -  49  (45) projelo %d aut. Mělo projet 20 153\n", gCounterArray[8]);
    Print("Na km  49 -  56  (53) projelo %d aut. Mělo projet 21 083\n", gCounterArray[9]);
    Print("Na km  56 -  66  (61) projelo %d aut. Mělo projet 20 532\n", gCounterArray[10]);
    Print("Na km  66 -  75  (70) projelo %d aut. Mělo projet 21 063\n", gCounterArray[11]);
    Print("Na km  75 -  81  (78) projelo %d aut. Mělo projet 21 062\n", gCounterArray[12]);
    Print("Na km  81 -  90  (85) projelo %d aut. Mělo projet 21 716\n", gCounterArray[13]);
    Print("Na km  90 - 104  (97) projelo %d aut. Mělo projet 21 005\n", gCounterArray[14]);
    Print("Na km 104 - 112 (108) projelo %d aut. Mělo projet 22 003\n", gCounterArray[15]);
    Print("Na km 112 - 119 (115) projelo %d aut. Mělo projet 22 209\n", gCounterArray[16]);
    Print("Na km 119 - 134 (127) projelo %d aut. Mělo projet 21 981\n", gCounterArray[17]);
    Print("Na km 134 - 141 (137) projelo %d aut. Mělo projet 22 400\n", gCounterArray[18]);
    Print("Na km 141 - 146 (143) projelo %d aut. Mělo projet 24 166\n", gCounterArray[19]);
    Print("Na km 146 - 153 (150) projelo %d aut. Mělo projet 23 485\n", gCounterArray[20]);
    Print("Na km 153 - 162 (157) projelo %d aut. Mělo projet 25 567\n", gCounterArray[21]);
    Print("Na km 162 - 168 (165) projelo %d aut. Mělo projet 27 462\n", gCounterArray[22]);
    Print("Na km 168 - 178 (173) projelo %d aut. Mělo projet 28 298\n", gCounterArray[23]);
    Print("Na km 178 - 182 (180) projelo %d aut. Mělo projet 28 774\n", gCounterArray[24]);
    Print("Na km 182 - 190 (189) projelo %d aut. Mělo projet 45 278\n", gCounterArray[25]);
    Print("Na km 190 - 194 (193) projelo %d aut. Mělo projet 49 659\n", gCounterArray[26]);
    Print("Na km 194 - 196 (195) projelo %d aut. Mělo projet 47 327\n", gCounterArray[27]);
    Print("Na km 196 - 201 (198) projelo %d aut. Mělo projet 51 584\n", gCounterArray[28]);
    Print("Na km 201 - 203 (202) projelo %d aut. Mělo projet 53 961\n\n", gCounterArray[29]);*/
#endif


    hZdrzeni.Output();
    hKilometry.Output();

    std::cout << std::endl;
	std::cout << "Nulty pred km: " << gEnd << std::endl;
	std::cout << "Nulty po km: " << gNasrat << std::endl;
    return 0;
}
// ========= MAIN ==========
// =========================
