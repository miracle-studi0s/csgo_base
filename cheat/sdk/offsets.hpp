#pragma once

#define FIND_OFFSET(image, offset) address_t{ image + XORN(offset) } 

// offsets were parsed at Sat Sep 16 13:38:30 2023

enum offsets_t : unsigned int {

	/*shaderapidx9.dll*/
	/*A1 ? ? ? ? 50 8B 08 FF 51 0C + 1*/
	OFFSET_D3D_DEVICE = 0x30A11,

	/*client.dll*/
	/*8B B6 ? ? ? ? 85 F6 74 05 83 3E 00 75 02 33 F6 F3 0F 10 44 24 + 2*/
	OFFSET_STUDIO_HDR = 0x1A1199,

	/*client.dll*/
	/*84 C0 74 ? 8B 87 ? ? ? ? 89 87*/
	OFFSET_RETURN_ADDR_MAINTAIN_SEQ_TRANSITIONS = 0x1CF88C,

	/*client.dll*/
	/*55 8B EC 51 56 8B F1 80 BE ?? ?? 00 00 00 74 36 8B 06 FF 90 ?? ?? 00 00*/
	OFFSET_UPDATE_CLIENT_SIDE_ANIMATION = 0x3E6300,

	/*client.dll*/
	/*0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9*/
	OFFSET_WORLD_TO_SCREEN_MATRIX = 0x21F4F8,

	/*client.dll*/
	/*56 8B 35 ? ? ? ? 85 F6 74 21 8B 41*/
	OFFSET_GET_NAME = 0x2084E0,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89*/
	OFFSET_LOAD_FROM_BUFFER = 0x9994A0,

	/*materialsystem.dll*/
	/*55 8B EC 83 EC ? 56 8B F1 8A 46*/
	OFFSET_GET_COLOR_MODULATION = 0xCF90,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 51 8B 45 18*/
	OFFSET_DRAW_MODELS = 0x31C110,

	/*engine.dll*/
	/*55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45*/
	OFFSET_LOAD_NAMED_SKY = 0x137780,

	/*client.dll*/
	/*80 3D ? ? ? ? ? 53 56 57 0F 85 + 2*/
	OFFSET_DISABLE_POST_PROCESS = 0x39B998,

	/*client.dll*/
	/*0F 11 05 ? ? ? ? 0F 10 87 + 3*/
	OFFSET_POST_PROCESS_DATA = 0x3F9C23,

	/*client.dll*/
	/*84 C0 75 0B 8B 0D ? ? ? ? 8B 01 FF 50 4C*/
	OFFSET_RETURN_ADDR_DRIFT_PITCH = 0x1B354A,

	/*client.dll*/
	/*84 C0 75 24 A1 ? ? ? ? B9 ? ? ? ? FF 50 1C A1 ? ? ? ? 51 C7 04 24 ? ? 80 3F B9 ? ? ? ? 53 57 FF 50 20*/
	OFFSET_RETURN_ADDR_APPLY_SHAKE = 0x1B36E2,

	/*client.dll*/
	/*55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0 + 8*/
	OFFSET_REMOVE_SMOKE = 0x3D0078,

	/*client.dll*/
	/*A3 ? ? ? ? 66 0F 6E 86 + 1*/
	OFFSET_PREDICTION_RANDOM_SEED = 0x3428EE,

	/*client.dll*/
	/*89 35 ? ? ? ? F3 0F 10 48 + 2*/
	OFFSET_PREDICTION_PLAYER = 0x342904,

	/*client.dll*/
	/*55 8B EC A1 ? ? ? ? 83 EC 20 8B 40 34*/
	OFFSET_ADD_VIEW_MODEL_BOB = 0x6A2BF0,

	/*client.dll*/
	/*55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9*/
	OFFSET_CALC_VIEW_MODEL_BOB = 0x1B3CB0,

	/*client.dll*/
	/*55 8B EC 56 8B F1 B9 ? ? ? ? C7 46*/
	OFFSET_CREATE_ANIMSTATE = 0x43CCE0,

	/*client.dll*/
	/*56 6A 01 68 ? ? ? ? 8B F1*/
	OFFSET_RESET_ANIMSTATE = 0x43CDF0,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24*/
	OFFSET_UPDATE_ANIMSTATE = 0x43D280,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8*/
	OFFSET_SET_ABS_ANGLES = 0x1E8FC0,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8*/
	OFFSET_SET_ABS_ORIGIN = 0x1E8E00,

	/*client.dll*/
	/*57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02*/
	OFFSET_SHOULD_SKIP_ANIM_FRAME = 0x1D16A0,

	/*client.dll*/
	/*55 8B EC 83 E4 F0 B8 D8*/
	OFFSET_SETUP_BONES = 0x1D1790,

	/*client.dll*/
	/*55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6*/
	OFFSET_STANDARD_BLENDING_RULES = 0x1CFDD0,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C*/
	OFFSET_DO_EXTRA_BONE_PROCESSING = 0x3E7720,

	/*client.dll*/
	/*84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80*/
	OFFSET_RETURN_ADDR_SETUP_VELOCITY = 0x441CF9,

	/*client.dll*/
	/*84 C0 75 0D F6 87*/
	OFFSET_RETURN_ADDR_ACCUMULATE_LAYERS = 0x3E6D46,

	/*client.dll*/
	/*84 C0 0F 85 ? ? ? ? A1 ? ? ? ? 8B B7*/
	OFFSET_RETURN_ADDR_REEVALUATE_ANIM_LOD = 0x3E6A5D,

	/*client.dll*/
	/*FF D0 A1 ? ? ? ? B9 ? ? ? ? D9 1D ? ? ? ? FF 50 34 85 C0 74 22 8B 0D ? ? ? ? + 29*/
	OFFSET_RETURN_ADDR_EXTRAPOLATION = 0x1E8245,

	/*client.dll*/
	/*84 C0 0F 84 ? ? ? ? 8B 44 24 24*/
	OFFSET_RETURN_ADDR_SETUP_BONES = 0x1D1F7A,

	/*client.dll*/
	/*80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81*/
	OFFSET_INVALIDATE_BONE_CACHE = 0x1D25E0,

	/*client.dll*/
	/*55 8B EC 83 EC 48 53 8B 5D 08 89 4D F4*/
	OFFSET_ATTACHMENTS_HELPER = 0x1D01F0,

	/*client.dll*/
	/*8B B7 ? ? ? ? 89 74 24 20*/
	OFFSET_STUDIO_HDR_PTR = 0x1D1B27,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56 83 E0 04*/
	OFFSET_INVALIDATE_PHYSICS_RECURSIVE = 0x1AD240,

	/*client.dll*/
	/*55 8B EC 83 E4 F0 83 EC 7C 56 52*/
	OFFSET_TRACE_FILTER = 0x1AF080,

	/*client.dll*/
	/*C7 45 ? ? ? ? ? 89 45 E4 8B 01*/
	OFFSET_TRACE_FILTER_SKIP_ENTITIES = 0x42B666,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 51 53 56 8B D9*/
	OFFSET_WRITE_USER_CMD = 0x374620,

	/*client.dll*/
	/*55 8B EC 53 56 8B F1 57 83 BE ? ? ? ? ? 75*/
	OFFSET_LOOKUP_BONE = 0x1CDF40,

	/*client.dll*/
	/*0F 11 05 ? ? ? ? 83 C8 01 C7 05 ? ? ? ? ? ? ? ? + 3*/
	OFFSET_GLOW_OBJECT = 0x2C7BA9,

	/*server.dll*/
	/*55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE*/
	OFFSET_DRAW_SERVER_HITBOX = 0x1D08A0,

	/*server.dll*/
	/*8B 15 ? ? ? ? 33 C9 83 7A 18 01*/
	OFFSET_GET_SERVER_EDICT = 0x247D67,

	/*client.dll*/
	/*E8 ? ? ? ? 85 C0 75 04 33 C0 5E C3 83*/
	/*relative*/
	OFFSET_MODEL_RENDERABLE_ANIMATING = 0x1CC703,

	/*client.dll*/
	/*55 8B EC 83 7D 08 ? 7E*/
	OFFSET_SETUP_CLR_MODULATION = 0x31C0A0,

	/*client.dll*/
	/*E8 ? ? ? ? 0F BF 87*/
	/*relative*/
	OFFSET_SET_COLLISION_BOUNDS = 0x33BEC7,

	/*client.dll*/
	/*55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28*/
	OFFSET_FIND_HUD_ELEMENT = 0x2D26C0,

	/*engine.dll*/
	/*55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 56 57 8B 7D 08 8B D9*/
	OFFSET_PROCESS_PACKET = 0x2561D0,

	/*gameoverlayrenderer.dll*/
	/*FF 15 ? ? ? ? 8B F0 85 FF + 2*/
	OFFSET_DIRECT_PRESENT = 0x66EF1,

	/*gameoverlayrenderer.dll*/
	/*C7 45 ? ? ? ? ? FF 15 ? ? ? ? 8B D8 + 9*/
	OFFSET_DIRECT_RESET = 0x67160,

	/*shaderapidx9.dll*/
	/*A1 ? ? ? ? 50 8B 08 FF 51 0C + 1*/
	OFFSET_DIRECT_DEVICE = 0x30A11,

	/*client.dll*/
	/*8B 0D ? ? ? ? 83 FF FF 74 07 + 2*/
	OFFSET_LOCAL_PLAYER = 0x1E1451,

	/*client.dll*/
	/*0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9*/
	OFFSET_SCREEN_MATRIX = 0x21F4F8,

	/*engine.dll*/
	/*53 8A D9 8B 0D ? ? ? ? 56 57 8B B9*/
	OFFSET_READ_PACKETS = 0xD99C0,

	/*client.dll*/
	/*55 8B EC 56 8B 75 08 57 FF 75 18*/
	OFFSET_ADD_RENDERABLE = 0x26EA70,

	/*client.dll*/
	/*55 8B EC 51 A1 ? ? ? ? 53 56 8B D9*/
	OFFSET_PERFORM_SCREEN_OVERLAY = 0x39FC50,

	/*client.dll*/
	/*53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B ? 89 6C ? ? 8B EC 83 EC ? 56 57 8B F9 89 7D ? 8B 4F*/
	OFFSET_RENDER_GLOW_BOXES = 0x2C7D80,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 F3 0F 10 42*/
	OFFSET_CLIP_RAY_TO_HITBOX = 0x7DC480,

	/*engine.dll*/
	/*85 C0 0F 95 C0 84 C0 75 0C*/
	OFFSET_READ_PACKETS_RETURN_ADDR = 0xD9AA4,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 81 EC 24 04 00 00 ? ? ? ? ? ?*/
	OFFSET_COMPUTE_HITBOX_SURROUNDING_BOX = 0x1D7BB0,

	/*engine.dll*/
	/*53 56 57 8B DA 8B F9 FF 15*/
	OFFSET_CLANTAG = 0x8DAB0,

	/*client.dll*/
	/*55 8B EC 51 56 8B F1 85 F6 74 68*/
	OFFSET_IS_BREAKABLE_ENTITY = 0x31F860,

	/*client.dll*/
	/*53 0F B7 1D ? ? ? ? 56*/
	OFFSET_PROCESS_INTERPOLATED_LIST = 0x1E7DC0,

	/*client.dll*/
	/*A2 ? ? ? ? 8B 45 E8*/
	OFFSET_ALLOW_EXTRAPOLATION = 0x1E82A8,

	/*client.dll*/
	/*55 8B EC 51 80 3D ? ? ? ? ? 0F 57*/
	OFFSET_GET_EXPOSURE_RANGE = 0x397810,

	/*engine.dll*/
	/*55 8B EC 83 E4 F8 83 EC 4C A1 ? ? ? ? 80*/
	OFFSET_TEMP_ENTITIES = 0x287130,

	/*client.dll*/
	/*E8 ? ? ? ? EB 38 83 EC 08*/
	/*relative*/
	OFFSET_CALC_SHOTGUN_SPREAD = 0x44CCEE,

	/*client.dll*/
	/*55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0*/
	OFFSET_LINE_GOES_THROUGH_SMOKE = 0x3D0070,

	/*engine.dll*/
	/*8B 0D ? ? ? ? 83 C1 04 8B 01 FF 37 FF 50 1C 89 47 10 + 2*/
	OFFSET_FILE_SYSTEM_PTR = 0x279B2,

	/*client.dll*/
	/*A1 ? ? ? ? 85 C0 75 53*/
	OFFSET_ITEM_SYSTEM = 0x7100B0,

	/*engine.dll*/
	/*A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85*/
	OFFSET_FORCE_UPDATE = 0xDE4D0,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 83 EC 24 53 56 57 8B F9 8D B7*/
	OFFSET_UPDATE_VISIBILITY = 0x1E5040,

	/*tier0.dll*/
	/*C6 86 ? ? ? ? ? 83 05 ? ? ? ? ? 5E 75 04 33 C0 87 07 + 2*/
	OFFSET_THREAD_ID_ALLOCATED = 0x12985,

	/*client.dll*/
	/*55 8B EC 56 8B 75 ? 57 8B F9 56 8B 07 FF 90*/
	OFFSET_WEAPON_SHOOTPOS = 0x42A1A0,

	/*engine.dll*/
	/*56 57 8B F9 8D 4F 08 C7 07 ? ? ? ? E8 ? ? ? ? C7*/
	OFFSET_CONSTRUCT_VOICE_DATA_MESSAGE = 0x19F550,

	/*client.dll*/
	/*55 8B EC 8B 45 ? 8B D1 83 F8 ? 0F 8C*/
	OFFSET_PUT_ATTACHMENTS = 0x1D00C0,

	/*client.dll*/
	/*55 8B EC 83 E4 ? 83 EC ? 80 3D ? ? ? ? ? 56 57 8B F9*/
	OFFSET_CALC_ABSOLUTE_POSITION = 0x1E9830,

	/*client.dll*/
	/*56 8B F1 85 F6 74 ? 8B 06 8B 80 ? ? ? ? FF D0 84 C0 74 ? 8A 86 ? ? ? ? 84 C0 74 ? 83 3D*/
	OFFSET_EYE_ANGLES = 0x3E66B0,

	/*client.dll*/
	/*B9 ? ? ? ? F3 0F 11 04 24 FF 50 10 + 1*/
	OFFSET_INPUT_PTR = 0x25F1DC,

	/*engine.dll*/
	/*A1 ? ? ? ? 8B 80 ? ? ? ? C3 + 1*/
	OFFSET_CLIENT_STATE = 0x25F81,

	/*client.dll*/
	/*8B 0D ? ? ? ? 8B 46 08 68 + 2*/
	OFFSET_MOVE_HELPER = 0x2BDCB5,

	/*client.dll*/
	/*A1 ? ? ? ? F3 0F 10 40 ? 0F 5A C0 F2 0F 11 04 + 1*/
	OFFSET_GLOBAL_VARS = 0x25EF57,

	/*client.dll*/
	/*8B 35 ? ? ? ? FF 10 0F B7 C0 + 2*/
	OFFSET_WEAPON_SYSTEM = 0x42F8CA,

	/*engine.dll*/
	/*55 8B EC 83 EC 40 53 8B D9 8B 0D ? ? ? ? 89 5D F8*/
	OFFSET_ENGINE_PAINT = 0x2B2110,

	/*client.dll*/
	/*A1 ? ? ? ? F6 44 + 1*/
	OFFSET_CLIENT_SIDE_ANIMATION_LIST = 0x1D87E1,

	/*client.dll*/
	/*55 8B EC 83 EC 1C 8B 0D ? ? ? ? 53 56*/
	OFFSET_INTERPOLATE_SERVER_ENTITIES = 0x1E8020,

	/*client.dll*/
	/*55 8B EC 8B 45 08 57 8B F9 8B 4F 04 85 C9 75 15 8B*/
	OFFSET_GET_POSE_PARAMETER = 0x36A330,

	/*client.dll*/
	/*89 86 ? ? ? ? E8 ? ? ? ? FF 75 08 + 2*/
	OFFSET_GET_BONE_MERGE = 0x1CE8B2,

	/*client.dll*/
	/*E8 ? ? ? ? 83 7E 10 ? 74 64*/
	/*relative*/
	OFFSET_UPDATE_MERGE_CACHE = 0x1BF986,

	/*client.dll*/
	/*E8 ? ? ? ? 8B 87 ? ? ? ? 8D 8C 24 ? ? ? ? 8B 7C 24 18*/
	/*relative*/
	OFFSET_COPY_TO_FOLLOW = 0x1DE5B6,

	/*client.dll*/
	/*E8 ? ? ? ? F3 0F 10 45 ? 8D 84 24 ? ? ? ?*/
	/*relative*/
	OFFSET_COPY_FROM_FOLLOW = 0x1DE54C,

	/*client.dll*/
	/*55 8B EC 81 EC BC ? ? ? 53 56 57*/
	OFFSET_ADD_DEPENDENCIES = 0x3AB110,

	/*engine.dll*/
	/*B8 ? ? ? ? 3B F0 0F 4F F0 89 5D FC*/
	OFFSET_SEND_MOVE_ADDR = 0xDD184,

	/*filesystem_stdio.dll*/
	/*55 8B EC 81 C1 ? ? ? ? 5D E9 ? ? ? ? CC 55 8B EC 8B 45 ? 89 41*/
	OFFSET_GET_UNVERIFIED_FILE_HASHES = 0xBA10,

	/*client.dll*/
	/*55 8B EC 83 E4 ? 81 EC ? ? ? ? 56 8B F1 57 8B 06*/
	OFFSET_CALC_CHASE_CAM_VIEW = 0x20A930,

	/*engine.dll*/
	/*E8 ? ? ? ? 3B F8 0F 4F C7 + 1*/
	OFFSET_START_SOUND_IMMEDIATE = 0x3C1DE,

	/*client.dll*/
	/*56 57 8B F9 BE ? ? ? ? 0F 1F 80*/
	OFFSET_INDEX_FROM_ANIM_TAG_NAME = 0x19BE60,

	/*client.dll*/
	/*55 8B EC 81 EC ? ? ? ? 53 56 57 8B 3D*/
	OFFSET_SETUP_AIM_MATRIX = 0x441490,

	/*client.dll*/
	/*55 8B EC 83 E4 ? 81 EC ? ? ? ? 56 57 8B 3D ? ? ? ? 8B F1*/
	OFFSET_SETUP_MOVEMENT = 0x43F590,

	/*client.dll*/
	/*55 8B EC 83 E4 ? 83 EC ? 53 56 57 83 CB*/
	OFFSET_POST_DATA_UPDATE = 0x208D70,

	/*client.dll*/
	/*55 8B EC 83 E4 ? 83 EC ? 56 8B F1 85 F6 74 ? 8B 06 8B 80 ? ? ? ? FF D0 84 C0 74 ? 8A 86*/
	OFFSET_ESTIMATE_ABS_VELOCITY = 0x1EB150,

	/*client.dll*/
	/*55 8B EC 8B 45 ? 85 C0 74 ? 80 B9 ? ? ? ? ? 74 ? 56 8B B1 ? ? ? ? 85 F6 74 ? 8D 4D ? 51 50 8B CE E8 ? ? ? ? 84 C0 74 ? 83 7D ? ? 75 ? F3 0F 10 45 ? F3 0F 11 86 ? ? ? ? 5E 5D C2 ? ? CC CC CC CC CC CC CC CC CC CC 55 8B EC 8B 45*/
	OFFSET_NOTIFY_ON_LAYER_CHANGE_WEIGHT = 0x3E6E90,

	/*client.dll*/
	/*55 8B EC 8B 45 ? 85 C0 74 ? 80 B9 ? ? ? ? ? 74 ? 8B 89*/
	OFFSET_NOTIFY_ON_LAYER_CHANGE_SEQUENCE = 0x3E6E60,

	/*client.dll*/
	/*55 8B EC 57 8B F9 8B 0D ? ? ? ? 8B 01 8B 80*/
	OFFSET_ACCUMULATE_LAYERS = 0x3E6D30,

	/*client.dll*/
	/*55 8B EC 83 EC ? 53 56 8B F1 57 80 BE ? ? ? ? ? 75 ? 8B 06*/
	OFFSET_ON_LATCH_INTERPOLATED_VARIABLES = 0x1E7350,

	/*client.dll*/
	/*55 8B EC 83 E4 ? 83 EC ? 53 56 8B F1 57 83 BE ? ? ? ? ? 75 ? 8B 46 ? 8D 4E ? FF 50 ? 85 C0 74 ? 8B CE E8 ? ? ? ? 8B 9E*/
	OFFSET_INTERPOLATE = 0x2119F0,

	/*client.dll*/
	/*55 8B EC 83 EC ? 56 8B F1 83 BE ? ? ? ? ? 0F 85*/
	OFFSET_INTERPOLATE_PLAYER = 0x1D4BF0,

	/*client.dll*/
	/*56 8B F1 57 8B BE ? ? ? ? 85 FF 74 ? 8B CF E8 ? ? ? ? 68*/
	OFFSET_RESET_LATCHED = 0x1D4BB0,

	/*client.dll*/
	/*0F 11 05 ? ? ? ? 83 C8 01 + 3*/
	OFFSET_GLOW_OBJECT_MANAGER = 0x2C7BA9,

	/*client.dll*/
	/*84 C0 75 05 B0*/
	OFFSET_RETURN_ADDR_LOADOUT_ALLOWED = 0x410C13,

	/*engine.dll*/
	/*8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? F3 0F 10 05 ? ? ? ? 0F 2E 05 ? ? ? ? 8B 15 ? ? ? ? 9F F6 C4 ? 7A ? 39 15 ? ? ? ? 75 ? A1 ? ? ? ? 33 05 ? ? ? ? A9 ? ? ? ? 74 ? 8B 0D ? ? ? ? 85 C9 74 ? 8B 01 68 ? ? ? ? FF 90 ? ? ? ? 8B 15 ? ? ? ? 8B 0D ? ? ? ? 81 F2 ? ? ? ? EB ? 8B 01 FF 50 ? 8B 0D ? ? ? ? 8B D0 83 FA ? 0F 85*/
	OFFSET_USING_STATIC_PROP_DEBUG = 0x28E4F0,

	/*client.dll*/
	/*55 8B EC 53 56 57 FF 75 ? 8B 7D*/
	OFFSET_BUILD_TRANSFORMATIONS = 0x3E7F60,

	/*client.dll*/
	/*55 8B EC 51 53 56 57 8B F9 8B 4D 04 E8 ? ? ? ? 8B*/
	OFFSET_UPDATE_POSTSCREEN_EFFECTS = 0x3F9730,

	/*client.dll*/
	/*55 8B EC 53 8B 5D 08 56 8B F1 83*/
	OFFSET_GET_SEQUENCE_ACTIVITY = 0x1D7B00,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 14 83 7F 60*/
	OFFSET_MODIFY_EYE_POSITION = 0x43D740,

	/*client.dll*/
	/*E8 ? ? ? ? 8B F0 EB 02 33 F6 8B 45 08 8B 78 + 1*/
	/*relative*/
	OFFSET_INIT_KEY_VALUES = 0x3FD171,

	/*engine.dll*/
	/*85 C0 74 0F 51 6A 00 56 8B C8 E8 ? ? ? ? 8B F0*/
	OFFSET_ALLOC_KEY_VALUES_ENGINE = 0xB92FA,

	/*client.dll*/
	/*FF 50 14 E8 ? ? ? ? 5F + FFFFFFF9*/
	OFFSET_VIEW_RENDER = 0x25F529,

	/*client.dll*/
	/*85 C0 74 10 6A 00 6A 00 56 8B C8 E8 ? ? ? ? 8B F0*/
	OFFSET_ALLOC_KEY_VALUES_CLIENT = 0x26576E,

	/*client.dll*/
	/*55 8B EC 8B 45 10 F3 0F 10 81*/
	OFFSET_ON_BBOX_CHANGE_CALLBACK = 0x430E10,

	/*client.dll*/
	/*55 8B EC 53 56 8B F1 8B 4D ? 57 E8*/
	OFFSET_WANT_RETICLE_SHOWN = 0x69F880,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 83 EC 2C 53 8B D9 56 57 8B*/
	OFFSET_UPDATE_ALL_VIEWMODEL_ADDONS = 0x2136E0,

	/*client.dll*/
	/*55 8B EC 8B 45 08 53 8B D9 56 8B 84 83 ? ? ? ? 83 F8 FF 74 1A 0F B7 F0*/
	OFFSET_GET_VIEWMODEL = 0x20C700,

	/*client.dll*/
	/*55 8B EC 83 EC 14 53 56 57 FF 75 18*/
	OFFSET_CALC_VIEW = 0x3DF830,

	/*client.dll*/
	/*B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08 + 1*/
	OFFSET_GET_HUD_PTR = 0x275A7A,

	/*client.dll*/
	/*53 56 8B F1 8A DA 8B 0D*/
	OFFSET_SHOULD_DRAW_FOG = 0x3A0FB0,

	/*engine.dll*/
	/*55 8B EC 81 EC 64 01 00 00 53 56 8A F9*/
	OFFSET_CL_MOVE = 0xDD290,

	/*client.dll*/
	/*55 8B EC 83 EC 0C 53 56 8B 71 58*/
	OFFSET_CLEAR_KILLFEED = 0x518700,

	/*client.dll*/
	/*56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 23*/
	OFFSET_PHYSICS_SIMULATE = 0x20C910,

	/*client.dll*/
	/*E8 ? ? ? ? 80 BE ? ? ? ? ? 0F 84 ? ? ? ? 83 BE ? ? ? ? ? 0F 84 ? ? ? ? B8*/
	/*relative*/
	OFFSET_CLAMP_BONES_IN_BBOX = 0x3E7FD9,

	/*client.dll*/
	/*55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 34*/
	OFFSET_CLEAR_HUD_WEAPONS = 0x558C80,

	/*engine.dll*/
	/*55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57 8B F9 89 7C 24 14*/
	OFFSET_SEND_DATAGRAM = 0x253D80,

	/*client.dll*/
	/*FF 50 18 89 44 24 14 EB + 3*/
	OFFSET_LIST_LEAVES_IN_BOX = 0x26F658,

	/*client.dll*/
	/*55 8B EC 83 EC 58 56 57*/
	OFFSET_CALC_VIEWMODEL_VIEW = 0x1B6580,

	/*client.dll*/
	/*85 C0 75 30 38 87*/
	OFFSET_RETURN_ADDR_CAM_THINK = 0x2F41BF,

	/*engine.dll*/
	/*55 8B EC 83 EC 08 56 8B F1 8B 4D 04*/
	OFFSET_SEND_NET_MSG = 0x256910,

	/*client.dll*/
	/*84 C0 74 ? 8D 8E ? ? ? ? E8 ? ? ? ? B9*/
	OFFSET_RETURN_ADDR_PROCESS_INPUT = 0x530C32,

	/*client.dll*/
	/*E8 ? ? ? ? 89 87 ? ? ? ? 6A*/
	/*relative*/
	OFFSET_VIEWMODEL_ARM_CFG = 0x3E4799,

	/*client.dll*/
	/*55 8B EC 83 B9 ? ? ? ? ? 75 0F*/
	OFFSET_TRACE_FILTER_TO_HEAD_COLLISION = 0x1AC500,

	/*client.dll*/
	/*FF 35 ? ? ? ? FF 90 ? ? ? ? 8B 8F + 2*/
	OFFSET_MASK_PTR = 0x3D3595,

	/*client.dll*/
	/*55 8B EC 83 EC ? 53 8B D9 8D 45 ? 8B 08*/
	OFFSET_UPDATE_ADDON_MODELS = 0x3DFC60,

	/*engine.dll*/
	/*55 8B EC 83 E4 ? 56 8B 35*/
	OFFSET_HOST_SHUTDOWN = 0x22EA90,

	/*engine.dll*/
	/*E8 ? ? ? ? 5E 8B E5 5D C3 CC CC CC CC CC CC CC CC CC CC CC CC 51*/
	/*relative*/
	OFFSET_DESTRUCT_VOICE_DATA_MESSAGE = 0x2289BA,

	/*engine.dll*/
	/*55 8B EC 83 E4 F8 A1 ? ? ? ? 81 EC 84 01 00*/
	OFFSET_MSG_VOICE_DATA = 0x2866A0,

	/*client.dll*/
	/*55 8B EC 83 EC 10 53 56 57 8B F9 8B 87*/
	OFFSET_PHYSICS_RUN_THINK = 0x336770,

	/*client.dll*/
	/*55 8B EC 56 57 8B F9 8B B7 ? ? ? ? 8B*/
	OFFSET_THINK = 0x1AC900,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9 56 57 83 BB ? ? ? ? ? 0F 84*/
	OFFSET_POST_THINK_PHYSICS = 0x1B5150,

	/*client.dll*/
	/*56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 74*/
	OFFSET_SIMULATE_PLAYER_SIMULATED_ENTITIES = 0x1B2100,

	/*client.dll*/
	/*A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 74 7A + 1*/
	OFFSET_GAME_RULES = 0x3F8BB7,

	/*engine.dll*/
	/*6A 00 8B 01 FF 90 B8 00 00 00 83 BF 08 01 + A*/
	OFFSET_RETURN_ADDR_SEND_DATAGRAM_CL_MOVE = 0xDD704,

	/*client.dll*/
	/*55 8B EC 83 E4 ? 81 EC ? ? ? ? F3 0F 7E 45*/
	OFFSET_FIRE_BULLET = 0x42C470,

	/*server.dll*/
	/*55 8B EC 8B 55 08 83 EC 30 56 8B F1 85 D2 0F 84 ? ? ? ? 8D 45 D0 8B C8 2B D1*/
	OFFSET_ADD_ACTIVITY_MODIFIER = 0x4B4940,

	/*client.dll*/
	/*53 56 57 8B F9 33 F6 8B 4F ? 8B 01 FF 90 ? ? ? ? 89 47*/
	OFFSET_GET_WEAPON_PREFIX = 0x442E90,

	/*client.dll*/
	/*55 8B EC 83 E4 ? 81 EC ? ? ? ? 53 56 57 8B F9 8B 17*/
	OFFSET_FIND_MAPPING = 0x36B760,

	/*client.dll*/
	/*55 8B EC 83 E4 ? 83 EC ? 53 56 8B 75 ? 8B D9 57 89 5C 24 ? 8B 16*/
	OFFSET_SELECT_SEQUENCE_FROM_MODS = 0x19B6F0,

	/*client.dll*/
	/*55 8B EC 56 8B 75 ? 57 8B F9 85 F6 78 ? 8B 47*/
	OFFSET_GET_SEQUENCE_DESC = 0x36A2A0,

	/*client.dll*/
	/*55 8B EC 56 8B F1 83 BE ? ? ? ? ? 75 ? 8B 46 ? 8D 4E ? FF 50 ? 85 C0 74 ? 8B CE E8 ? ? ? ? 8B B6 ? ? ? ? 85 F6 74 ? 83 3E ? 74 ? 8B CE E8 ? ? ? ? 84 C0 74 ? FF 75*/
	OFFSET_LOOKUP_SEQUENCE = 0x1D7790,

	/*client.dll*/
	/*55 8B EC 83 EC ? 56 8B F1 57 8B FA 85 F6 75 ? 68*/
	OFFSET_GET_SEQUENCE_LINEAR_MOTION = 0x19BBB0,

	/*client.dll*/
	/*55 8B EC 51 53 56 57 8B F9 83 7F ? ? 0F 84*/
	OFFSET_UPDATE_LAYER_ORDER_PRESET = 0x4413F0,

	/*client.dll*/
	/*53 8B D9 F6 C3 03 74 0B FF 15 ? ? ? ? 84 C0 74 01 ? C7 83 ? ? ? ? ? ? ? ? 8B CB*/
	OFFSET_IK_CONTEXT_CONSTRUCT = 0x7E6760,

	/*client.dll*/
	/*56 8B F1 57 8D 8E ? ? ? ? E8 ? ? ? ? 8D 8E ? ? ? ? E8 ? ? ? ? 83 BE ? ? ? ? ?*/
	OFFSET_IK_CONTEXT_DESTRUCT = 0x1CD510,

	/*client.dll*/
	/*55 8B EC 83 EC 08 8B 45 08 56 57 8B F9 8D 8F*/
	OFFSET_IK_CONTEXT_INIT = 0x7E6830,

	/*client.dll*/
	/*E8 ? ? ? ? 8B 47 FC 8D 4F FC F3 0F 10 44 24*/
	/*relative*/
	OFFSET_IK_CONTEXT_UPDATE_TARGETS = 0x1D228E,

	/*client.dll*/
	/*E8 ? ? ? ? 8B 44 24 40 8B 4D 0C*/
	/*relative*/
	OFFSET_IK_CONTEXT_SOLVE_DEPENDENCIES = 0x9AB33C,

	/*client.dll*/
	/*55 8B EC 83 EC 10 53 8B D9 89 55 F8 56 57 89 5D F4 8B 0B 89 4D F0*/
	OFFSET_INIT_POSE = 0x7E1790,

	/*client.dll*/
	/*55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? A1*/
	OFFSET_ACCUMULATE_POSE = 0x7E28F0,

	/*client.dll*/
	/*55 8B EC 83 EC 10 53 56 57 8B 7D 10 8B D9 F3 0F 11 5D*/
	OFFSET_CALC_AUTOPLAY_SEQUENCES = 0x7E3410,

	/*client.dll*/
	/*55 8B EC 83 E4 F8 81 EC ? ? ? ? 8B C1 89 54 24 04 89 44 24 2C 56 57 8B*/
	OFFSET_CALC_BONE_ADJUST = 0x7E2DB0,

	/*client.dll*/
	/*55 8B EC 83 EC ? 56 8B F1 8B 06 8B 80 ? ? ? ? FF D0 84 C0 75*/
	OFFSET_NOTE_PREDICTION_ERROR = 0x20D030,

	/*client.dll*/
	/*55 8B EC 83 EC ? 56 8B 75 ? F6 86*/
	OFFSET_CHECK_MOVING_GROUND = 0x342740,

	/*client.dll*/
	/*B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9 + 1*/
	OFFSET_VIEW_RENDER_BEAMS = 0x38A13A,

	/*client.dll*/
	/*84 C0 0F 85 ? ? ? ? 8B CB E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 83 BF*/
	OFFSET_RETURN_ADDR_SHOW_RADAR = 0x52DB05,

	/*client.dll*/
	/*55 8B EC 83 E4 ? 81 EC ? ? ? ? 53 8B D9 8D 4C 24*/
	OFFSET_CALC_ROAMING_VIEW = 0x20AEF0,

	/*client.dll*/
	/*85 D2 75 ? E8 ? ? ? ? 8B C8*/
	OFFSET_RETURN_ADDR_POST_PROCESS = 0x39BBF6,

	/*client.dll*/
	/*55 8B EC 8B 45 ? 85 C0 74 ? 80 B9 ? ? ? ? ? 74 ? 56 8B B1 ? ? ? ? 85 F6 74 ? 8D 4D ? 51 50 8B CE E8 ? ? ? ? 84 C0 74 ? 83 7D ? ? 75 ? F3 0F 10 45 ? F3 0F 11 86 ? ? ? ? 5E 5D C2 ? ? CC CC CC CC CC CC CC CC CC CC 55 8B EC 53*/
	OFFSET_NOTIFY_ON_LAYER_CHANGE_CYCLE = 0x3E6EE0,

	/*client.dll*/
	/*55 8B EC 51 53 56 57 8B F9 8B 77 ? 83 BE*/
	OFFSET_SETUP_WEAPON_ACTION = 0x43F380,

	/*client.dll*/
	/*55 8B EC 83 EC ? 56 57 8B F9 8B 77*/
	OFFSET_SETUP_WHOLE_BODY_ACTION = 0x43F1D0,

	/*client.dll*/
	/*55 8B EC 51 56 8B 71 ? 83 BE ? ? ? ? ? 0F 84 ? ? ? ? 8B B6 ? ? ? ? 81 C6 ? ? ? ? 0F 84 ? ? ? ? F3 0F 10 56 ? 0F 28 C2 E8 ? ? ? ? 0F 57 DB 0F 2F D8 73 ? F3 0F 10 49 ? F3 0F 10 66 ? F3 0F 59 CA F3 0F 10 15*/
	OFFSET_SETUP_FLINCH = 0x43F110,

	/*client.dll*/
	/*55 8B EC 83 E4 ? 83 EC ? 53 56 8B F1 57 8B 46*/
	OFFSET_CACHE_SEQUENCES = 0x443010,

};
