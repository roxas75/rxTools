#ifndef DOWNGRADEAPP_H
#define DOWNGRADEAPP_H

//Utilities
char* getContentAppPath();
char* getTMDAppPath();
int FindApp(unsigned int tid_low, unsigned int tid_high, int drive);
int CheckRegion(int drive);

//Features
void downgradeMSET();
void installFBI();
void restoreHS();

#endif
