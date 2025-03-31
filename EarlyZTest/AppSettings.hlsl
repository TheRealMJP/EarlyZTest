#pragma once

struct AppSettings_CBLayout
{
    bool EnableDepthWrites;
    bool ReverseTriangleOrder;
};

ConstantBuffer<AppSettings_CBLayout> AppSettingsCB : register(b12);

struct AppSettings_Values
{
    bool EnableDepthWrites;
    bool ReverseTriangleOrder;
    int DiscardMode;
    int DepthExportMode;
    int UAVWriteMode;
    bool ForceEarlyZ;
};

static const AppSettings_Values AppSettings =
{
    AppSettingsCB.EnableDepthWrites,
    AppSettingsCB.ReverseTriangleOrder,
    DiscardMode_,
    DepthExportMode_,
    UAVWriteMode_,
    ForceEarlyZ_,
};

enum DiscardModes
{
    NoDiscard = 0,
    DiscardChecker = 1,
    DiscardNever = 2,
};

#define NoDiscard_ 0
#define DiscardChecker_ 1
#define DiscardNever_ 2

enum DepthExportModes
{
    NoDepthExport = 0,
    ArbitraryDepth = 1,
    ConservativeDepthMatching = 2,
    ConservativeDepthOpposing = 3,
};

#define NoDepthExport_ 0
#define ArbitraryDepth_ 1
#define ConservativeDepthMatching_ 2
#define ConservativeDepthOpposing_ 3

enum UAVWriteModes
{
    NoUAV = 0,
    StandardUAV = 1,
    ROV = 2,
};

#define NoUAV_ 0
#define StandardUAV_ 1
#define ROV_ 2

