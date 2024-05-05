static constexpr int ALLTAPS = 483;
static constexpr float coefs[]={
0.00010319,
0.00011006,
0.00011708,
0.00012431,
0.00013164,
0.00013906,
0.00014670,
0.00015451,
0.00016255,
0.00017068,
0.00017888,
0.00018733,
0.00019594,
0.00020482,
0.00021378,
0.00022278,
0.00023207,
0.00024152,
0.00025127,
0.00026106,
0.00027090,
0.00028104,
0.00029135,
0.00030199,
0.00031265,
0.00032333,
0.00033434,
0.00034553,
0.00035707,
0.00036862,
0.00038016,
0.00039206,
0.00040414,
0.00041660,
0.00042904,
0.00044145,
0.00045424,
0.00046722,
0.00048061,
0.00049395,
0.00050723,
0.00052092,
0.00053480,
0.00054913,
0.00056337,
0.00057751,
0.00059210,
0.00060687,
0.00062213,
0.00063727,
0.00065227,
0.00066775,
0.00068341,
0.00069960,
0.00071562,
0.00073146,
0.00074782,
0.00076436,
0.00078146,
0.00079835,
0.00081501,
0.00083222,
0.00084962,
0.00086761,
0.00088535,
0.00090280,
0.00092085,
0.00093908,
0.00095793,
0.00097648,
0.00099470,
0.00101355,
0.00103257,
0.00105226,
0.00107159,
0.00109053,
0.00111014,
0.00112992,
0.00115040,
0.00117046,
0.00119008,
0.00121041,
0.00123090,
0.00125213,
0.00127288,
0.00129313,
0.00131413,
0.00133527,
0.00135720,
0.00137858,
0.00139940,
0.00142101,
0.00144275,
0.00146531,
0.00148727,
0.00150860,
0.00153075,
0.00155302,
0.00157616,
0.00159863,
0.00162039,
0.00164301,
0.00166575,
0.00168940,
0.00171230,
0.00173442,
0.00175745,
0.00178057,
0.00180465,
0.00182790,
0.00185030,
0.00187366,
0.00189709,
0.00192151,
0.00194504,
0.00196764,
0.00199123,
0.00201488,
0.00203957,
0.00206328,
0.00208599,
0.00210973,
0.00213352,
0.00215837,
0.00218218,
0.00220491,
0.00222871,
0.00225253,
0.00227747,
0.00230128,
0.00232392,
0.00234768,
0.00237145,
0.00239636,
0.00242008,
0.00244255,
0.00246617,
0.00248978,
0.00251457,
0.00253809,
0.00256028,
0.00258366,
0.00260701,
0.00263159,
0.00265480,
0.00267662,
0.00269966,
0.00272265,
0.00274690,
0.00276972,
0.00279106,
0.00281365,
0.00283618,
0.00286000,
0.00288231,
0.00290306,
0.00292511,
0.00294707,
0.00297036,
0.00299206,
0.00301213,
0.00303353,
0.00305482,
0.00307748,
0.00309846,
0.00311776,
0.00313840,
0.00315893,
0.00318084,
0.00320101,
0.00321943,
0.00323922,
0.00325888,
0.00327995,
0.00329922,
0.00331665,
0.00333550,
0.00335419,
0.00337433,
0.00339259,
0.00340896,
0.00342677,
0.00344441,
0.00346351,
0.00348068,
0.00349590,
0.00351258,
0.00352907,
0.00354705,
0.00356304,
0.00357702,
0.00359248,
0.00360775,
0.00362452,
0.00363925,
0.00365192,
0.00366609,
0.00368005,
0.00369554,
0.00370893,
0.00372021,
0.00373302,
0.00374560,
0.00375973,
0.00377171,
0.00378154,
0.00379292,
0.00380406,
0.00381676,
0.00382728,
0.00383560,
0.00384549,
0.00385513,
0.00386634,
0.00387534,
0.00388211,
0.00389045,
0.00389853,
0.00390821,
0.00391564,
0.00392081,
0.00392757,
0.00393405,
0.00394214,
0.00394796,
0.00395150,
0.00395664,
0.00396150,
0.00396797,
0.00397215,
0.00397403,
0.00397751,
0.00398072,
0.00398554,
0.00398806,
0.00398827,
0.00399009,
0.00399162,
0.00399477,
0.00399561,
0.00399414,
0.00399428,
0.00399414,
0.00399561,
0.00399477,
0.00399162,
0.00399009,
0.00398827,
0.00398806,
0.00398554,
0.00398072,
0.00397751,
0.00397403,
0.00397215,
0.00396797,
0.00396150,
0.00395664,
0.00395150,
0.00394796,
0.00394214,
0.00393405,
0.00392757,
0.00392081,
0.00391564,
0.00390821,
0.00389853,
0.00389045,
0.00388211,
0.00387534,
0.00386634,
0.00385513,
0.00384549,
0.00383560,
0.00382728,
0.00381676,
0.00380406,
0.00379292,
0.00378154,
0.00377171,
0.00375973,
0.00374560,
0.00373302,
0.00372021,
0.00370893,
0.00369554,
0.00368005,
0.00366609,
0.00365192,
0.00363925,
0.00362452,
0.00360775,
0.00359248,
0.00357702,
0.00356304,
0.00354705,
0.00352907,
0.00351258,
0.00349590,
0.00348068,
0.00346351,
0.00344441,
0.00342677,
0.00340896,
0.00339259,
0.00337433,
0.00335419,
0.00333550,
0.00331665,
0.00329922,
0.00327995,
0.00325888,
0.00323922,
0.00321943,
0.00320101,
0.00318084,
0.00315893,
0.00313840,
0.00311776,
0.00309846,
0.00307748,
0.00305482,
0.00303353,
0.00301213,
0.00299206,
0.00297036,
0.00294707,
0.00292511,
0.00290306,
0.00288231,
0.00286000,
0.00283618,
0.00281365,
0.00279106,
0.00276972,
0.00274690,
0.00272265,
0.00269966,
0.00267662,
0.00265480,
0.00263159,
0.00260701,
0.00258366,
0.00256028,
0.00253809,
0.00251457,
0.00248978,
0.00246617,
0.00244255,
0.00242008,
0.00239636,
0.00237145,
0.00234768,
0.00232392,
0.00230128,
0.00227747,
0.00225253,
0.00222871,
0.00220491,
0.00218218,
0.00215837,
0.00213352,
0.00210973,
0.00208599,
0.00206328,
0.00203957,
0.00201488,
0.00199123,
0.00196764,
0.00194504,
0.00192151,
0.00189709,
0.00187366,
0.00185030,
0.00182790,
0.00180465,
0.00178057,
0.00175745,
0.00173442,
0.00171230,
0.00168940,
0.00166575,
0.00164301,
0.00162039,
0.00159863,
0.00157616,
0.00155302,
0.00153075,
0.00150860,
0.00148727,
0.00146531,
0.00144275,
0.00142101,
0.00139940,
0.00137858,
0.00135720,
0.00133527,
0.00131413,
0.00129313,
0.00127288,
0.00125213,
0.00123090,
0.00121041,
0.00119008,
0.00117046,
0.00115040,
0.00112992,
0.00111014,
0.00109053,
0.00107159,
0.00105226,
0.00103257,
0.00101355,
0.00099470,
0.00097648,
0.00095793,
0.00093908,
0.00092085,
0.00090280,
0.00088535,
0.00086761,
0.00084962,
0.00083222,
0.00081501,
0.00079835,
0.00078146,
0.00076436,
0.00074782,
0.00073146,
0.00071562,
0.00069960,
0.00068341,
0.00066775,
0.00065227,
0.00063727,
0.00062213,
0.00060687,
0.00059210,
0.00057751,
0.00056337,
0.00054913,
0.00053480,
0.00052092,
0.00050723,
0.00049395,
0.00048061,
0.00046722,
0.00045424,
0.00044145,
0.00042904,
0.00041660,
0.00040414,
0.00039206,
0.00038016,
0.00036862,
0.00035707,
0.00034553,
0.00033434,
0.00032333,
0.00031265,
0.00030199,
0.00029135,
0.00028104,
0.00027090,
0.00026106,
0.00025127,
0.00024152,
0.00023207,
0.00022278,
0.00021378,
0.00020482,
0.00019594,
0.00018733,
0.00017888,
0.00017068,
0.00016255,
0.00015451,
0.00014670,
0.00013906,
0.00013164,
0.00012431,
0.00011708,
0.00011006,
0.00010319,
};