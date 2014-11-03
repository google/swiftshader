#ifndef __rad_h_
#define __rad_h_

/*
** Copyright 1998-2014, NVIDIA Corporation.
** All Rights Reserved.
** 
** THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
** NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
** IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
*/

#ifndef RADAPIENTRY
# ifdef _WIN32
#  if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)  /* Mimic <windef.h> */
#   define RADAPIENTRY __stdcall
#  else
#   define RADAPIENTRY
#  endif
# else
#  define RADAPIENTRY
# endif
#endif

#ifndef RADAPI
# if defined(__GNUC__)
#  define RADAPI extern __attribute__ ((visibility ("default")))
# else
#  define RADAPI extern
# endif
#endif

#ifndef RADAPIENTRYP
# define RADAPIENTRYP RADAPIENTRY *
#endif

/*************************************************************/

typedef unsigned char RADboolean;
typedef unsigned int RADbitfield;
typedef signed char RADbyte;
typedef short RADshort;
typedef int RADint;
typedef int RADsizei;
typedef unsigned char RADubyte;
typedef unsigned short RADushort;
typedef unsigned int RADuint;
typedef float RADfloat;
typedef double RADdouble;
typedef void RADvoid;

# if defined(_WIN64)
    typedef __int64 RADintptr;
    typedef __int64 RADsizeiptr;
# elif defined(__LP64__)
    typedef long int RADintptr;
    typedef long int RADsizeiptr;
# else
    typedef int RADintptr;
    typedef int RADsizeiptr;
# endif
typedef char RADchar;
typedef signed long long RADint64;
typedef unsigned long long RADuint64;

typedef struct __RADdevice *RADdevice;
typedef struct __RADqueue *RADqueue;
typedef struct __RADpipeline *RADpipeline;
typedef struct __RADcommandBuffer *RADcommandBuffer;
typedef struct __RADcolorState *RADcolorState;
typedef struct __RADdepthStencilState *RADdepthStencilState;
typedef struct __RADvertexState *RADvertexState;
typedef struct __RADrasterState *RADrasterState;
typedef struct __RADrtFormatState *RADrtFormatState;
typedef struct __RADprogram *RADprogram;
typedef struct __RADbuffer *RADbuffer;
typedef struct __RADtexture *RADtexture;
typedef struct __RADsampler *RADsampler;
typedef struct __RADpass *RADpass;
typedef struct __RADsync *RADsync;

typedef struct RADbindGroupElement {
    RADuint64 handle;
    RADuint   offset;
    RADuint   length;
} RADbindGroupElement;

typedef struct RADoffset2D {
    RADint x, y;
} RADoffset2D;

typedef struct RADrect2D {
    RADint x, y, width, height;
} RADrect2D;

typedef RADuint64 RADvertexHandle;
typedef RADuint64 RADindexHandle;
typedef RADuint64 RADuniformHandle;
typedef RADuint64 RADpipelineHandle;
typedef RADuint64 RADcommandHandle;
typedef RADuint64 RADtextureHandle;
typedef RADuint64 RADrenderTargetHandle;
typedef RADuint64 RADbindGroupHandle;
typedef void (*RADPROC)(void);

typedef enum {
    RAD_TAG_AUTO = 0,
    RAD_TAG_MANUAL = 1,
} RADtagMode;

typedef enum {
    RAD_QUEUE_TYPE_GRAPHICS_AND_COMPUTE = 0,
    RAD_QUEUE_TYPE_GRAPHICS = 1,
    RAD_QUEUE_TYPE_COMPUTE = 2,
    RAD_QUEUE_TYPE_COPY = 3,
} RADqueueType;

typedef enum {
    RAD_PIPELINE_TYPE_GRAPHICS = 0,
} RADpipelineType;

typedef enum {
    RAD_PROGRAM_FORMAT_GLSL = 0,
} RADprogramFormat;

typedef enum {
    RAD_TEXTURE_1D = 0x0000,
    RAD_TEXTURE_2D = 0x0001,
    RAD_TEXTURE_3D = 0x0002,
    RAD_TEXTURE_1D_ARRAY = 0x0003,
    RAD_TEXTURE_2D_ARRAY = 0x0004,
    RAD_TEXTURE_2D_MULTISAMPLE = 0x0005,
    RAD_TEXTURE_2D_MULTISAMPLE_ARRAY = 0x0006,
    RAD_TEXTURE_RECTANGLE = 0x0007,
    RAD_TEXTURE_CUBEMAP = 0x0008,
    RAD_TEXTURE_CUBEMAP_ARRAY = 0x0009,
} RADtextureTarget;

typedef enum {
	RAD_FORMAT_NONE = 0x000,
    RAD_RGBA8 = 0x8058,
    RAD_DEPTH24_STENCIL8 = 0x88F0,
} RADinternalFormat;

typedef enum {
    RAD_BLEND_FUNC_ZERO = 0x0000,
    RAD_BLEND_FUNC_ONE = 0x0001,
    RAD_BLEND_FUNC_SRC_COLOR = 0x0002,
    RAD_BLEND_FUNC_ONE_MINUS_SRC_COLOR = 0x0003,
    RAD_BLEND_FUNC_DST_COLOR = 0x0004,
    RAD_BLEND_FUNC_ONE_MINUS_DST_COLOR = 0x0005,
    RAD_BLEND_FUNC_SRC_ALPHA = 0x0006,
    RAD_BLEND_FUNC_ONE_MINUS_SRC_ALPHA = 0x0007,
    RAD_BLEND_FUNC_DST_ALPHA = 0x0008,
    RAD_BLEND_FUNC_ONE_MINUS_DST_ALPHA = 0x0009,
    RAD_BLEND_FUNC_SRC_ALPHA_SATURATE = 0x000A,
    RAD_BLEND_FUNC_CONSTANT_COLOR = 0x000B,
    RAD_BLEND_FUNC_ONE_MINUS_CONSTANT_COLOR = 0x000C,
    RAD_BLEND_FUNC_CONSTANT_ALPHA = 0x000D,
    RAD_BLEND_FUNC_ONE_MINUS_CONSTANT_ALPHA = 0x000E,
    RAD_BLEND_FUNC_SRC1_COLOR = 0x000F,
    RAD_BLEND_FUNC_ONE_MINUS_SRC1_COLOR = 0x0010,
    RAD_BLEND_FUNC_SRC1_ALPHA = 0x0011,
    RAD_BLEND_FUNC_ONE_MINUS_SRC1_ALPHA = 0x0012,
} RADblendFunc;

typedef enum {
    RAD_BLEND_EQUATION_ADD = 0x0000,
    RAD_BLEND_EQUATION_MIN = 0x0001,
    RAD_BLEND_EQUATION_MAX = 0x0002,
    RAD_BLEND_EQUATION_SUB = 0x0003,
    RAD_BLEND_EQUATION_REVERSE_SUB = 0x0004,
} RADblendEquation;

typedef enum {
    RAD_LOGIC_OP_CLEAR = 0x0000,
    RAD_LOGIC_OP_AND = 0x0001,
    RAD_LOGIC_OP_AND_REVERSE = 0x0002,
    RAD_LOGIC_OP_COPY = 0x0003,
    RAD_LOGIC_OP_AND_INVERTED = 0x0004,
    RAD_LOGIC_OP_NOOP = 0x0005,
    RAD_LOGIC_OP_XOR = 0x0006,
    RAD_LOGIC_OP_OR = 0x0007,
    RAD_LOGIC_OP_NOR = 0x0008,
    RAD_LOGIC_OP_EQUIV = 0x0009,
    RAD_LOGIC_OP_INVERT = 0x000A,
    RAD_LOGIC_OP_OR_REVERSE = 0x000B,
    RAD_LOGIC_OP_COPY_INVERTED = 0x000C,
    RAD_LOGIC_OP_OR_INVERTED = 0x000D,
    RAD_LOGIC_OP_NAND = 0x000E,
    RAD_LOGIC_OP_SET = 0x000F,
} RADlogicOp;

typedef enum {
    RAD_COLOR_DYNAMIC_BLEND_COLOR = 0x0000,
} RADcolorDynamic;

typedef enum {
    RAD_ATTRIB_TYPE_SNORM = 0x0000,
    RAD_ATTRIB_TYPE_UNORM = 0x0001,
    RAD_ATTRIB_TYPE_INT = 0x0002,
    RAD_ATTRIB_TYPE_UNSIGNED_INT = 0x0003,
    RAD_ATTRIB_TYPE_INT_TO_FLOAT = 0x0004,
    RAD_ATTRIB_TYPE_UNSIGNED_INT_TO_FLOAT = 0x0005,
    RAD_ATTRIB_TYPE_FLOAT = 0x0006,
    RAD_ATTRIB_TYPE_SNORM_2_10_10_10_REV = 0x0007,
    RAD_ATTRIB_TYPE_UNORM_2_10_10_10_REV = 0x0008,
    RAD_ATTRIB_TYPE_INT_TO_FLOAT_2_10_10_10_REV = 0x0009,
    RAD_ATTRIB_TYPE_UNSIGNED_INT_TO_FLOAT_2_10_10_10_REV = 0x000A,
    RAD_ATTRIB_TYPE_10F_11F_11F_REV = 0x000B,
} RADattribType;

typedef enum {
    RAD_TRIANGLES = 0x0004,
} RADprimitiveType;

typedef enum {
    RAD_INDEX_UNSIGNED_BYTE = 0x0000,
    RAD_INDEX_UNSIGNED_SHORT = 0x0001,
    RAD_INDEX_UNSIGNED_INT = 0x0002,
} RADindexType;

typedef enum {
    RAD_DEPTH_FUNC_NEVER = 0x0000,
    RAD_DEPTH_FUNC_LESS = 0x0001,
    RAD_DEPTH_FUNC_EQUAL = 0x0002,
    RAD_DEPTH_FUNC_LEQUAL = 0x0003,
    RAD_DEPTH_FUNC_GREATER = 0x0004,
    RAD_DEPTH_FUNC_NOTEQUAL = 0x0005,
    RAD_DEPTH_FUNC_GEQUAL = 0x0006,
    RAD_DEPTH_FUNC_ALWAYS = 0x0007,
} RADdepthFunc;

typedef enum {
    RAD_MAG_FILTER_NEAREST = 0x0000,
    RAD_MAG_FILTER_LINEAR = 0x0001,
} RADmagFilter;

typedef enum {
    RAD_MIN_FILTER_NEAREST = 0x0000,
    RAD_MIN_FILTER_LINEAR = 0x0001,
    RAD_MIN_FILTER_NEAREST_MIPMAP_NEAREST = 0x0002,
    RAD_MIN_FILTER_LINEAR_MIPMAP_NEAREST = 0x0003,
    RAD_MIN_FILTER_NEAREST_MIPMAP_LINEAR = 0x0004,
    RAD_MIN_FILTER_LINEAR_MIPMAP_LINEAR = 0x0005,
} RADminFilter;

// TODO: Figure out the correct subset of wrap modes to support
typedef enum {
    RAD_WRAP_MODE_CLAMP = 0x0000,
    RAD_WRAP_MODE_REPEAT = 0x0001,
    RAD_WRAP_MODE_MIRROR_CLAMP = 0x0002,
    RAD_WRAP_MODE_MIRROR_CLAMP_TO_EDGE = 0x0003,
    RAD_WRAP_MODE_MIRROR_CLAMP_TO_BORDER = 0x0004,
    RAD_WRAP_MODE_CLAMP_TO_BORDER = 0x0005,
    RAD_WRAP_MODE_MIRRORED_REPEAT = 0x0006,
    RAD_WRAP_MODE_CLAMP_TO_EDGE = 0x0007,
} RADwrapMode;

typedef enum {
    RAD_COMPARE_MODE_NONE = 0x0000,
    RAD_COMPARE_MODE_COMPARE_R_TO_TEXTURE = 0x0001,
} RADcompareMode;

typedef enum {
    RAD_COMPARE_FUNC_NEVER = 0x0000,
    RAD_COMPARE_FUNC_LESS = 0x0001,
    RAD_COMPARE_FUNC_EQUAL = 0x0002,
    RAD_COMPARE_FUNC_LEQUAL = 0x0003,
    RAD_COMPARE_FUNC_GREATER = 0x0004,
    RAD_COMPARE_FUNC_NOTEQUAL = 0x0005,
    RAD_COMPARE_FUNC_GEQUAL = 0x0006,
    RAD_COMPARE_FUNC_ALWAYS = 0x0007,
} RADcompareFunc;

typedef enum {
    RAD_FACE_NONE = 0x0000,
    RAD_FACE_FRONT = 0x0001,
    RAD_FACE_BACK = 0x0002,
    RAD_FACE_FRONT_AND_BACK = 0x0003,
} RADfaceBitfield;

typedef enum {
    RAD_STENCIL_FUNC_NEVER = 0x0000,
    RAD_STENCIL_FUNC_LESS = 0x0001,
    RAD_STENCIL_FUNC_EQUAL = 0x0002,
    RAD_STENCIL_FUNC_LEQUAL = 0x0003,
    RAD_STENCIL_FUNC_GREATER = 0x0004,
    RAD_STENCIL_FUNC_NOTEQUAL = 0x0005,
    RAD_STENCIL_FUNC_GEQUAL = 0x0006,
    RAD_STENCIL_FUNC_ALWAYS = 0x0007,
} RADstencilFunc;

typedef enum {
    RAD_STENCIL_OP_KEEP = 0x0000,
    RAD_STENCIL_OP_ZERO = 0x0001,
    RAD_STENCIL_OP_REPLACE = 0x0002,
    RAD_STENCIL_OP_INCR = 0x0003,
    RAD_STENCIL_OP_DECR = 0x0004,
    RAD_STENCIL_OP_INVERT = 0x0005,
    RAD_STENCIL_OP_INCR_WRAP = 0x0006,
    RAD_STENCIL_OP_DECR_WRAP = 0x0007,
} RADstencilOp;

typedef enum {
    // Both faces must either be dynamic or static
    RAD_DEPTH_STENCIL_DYNAMIC_STENCIL_VALUE_MASK = 0x0000,
    RAD_DEPTH_STENCIL_DYNAMIC_STENCIL_MASK = 0x0001,
    RAD_DEPTH_STENCIL_DYNAMIC_STENCIL_REF = 0x0002,
} RADdepthStencilDynamic;


typedef enum {
    RAD_FRONT_FACE_CW = 0x0000,
    RAD_FRONT_FACE_CCW = 0x0001,
} RADfrontFace;

typedef enum {
    RAD_POLYGON_MODE_POINT = 0x0000,
    RAD_POLYGON_MODE_LINE = 0x0001,
    RAD_POLYGON_MODE_FILL = 0x0002,
} RADpolygonMode;

typedef enum {
	RAD_POLYGON_OFFSET_NONE = 0x0000,
    RAD_POLYGON_OFFSET_POINT = 0x0001,
    RAD_POLYGON_OFFSET_LINE = 0x0002,
    RAD_POLYGON_OFFSET_FILL = 0x0004,
} RADpolygonOffsetEnables;

typedef enum {
    RAD_RASTER_DYNAMIC_POINT_SIZE = 0x0000,
    RAD_RASTER_DYNAMIC_LINE_WIDTH = 0x0001,
    RAD_RASTER_DYNAMIC_POLYGON_OFFSET_CLAMP = 0x0002,
    RAD_RASTER_DYNAMIC_SAMPLEMASK = 0x0003,
} RADrasterDynamic;

typedef enum {
    RAD_RT_ATTACHMENT_DEPTH = 0x0000,
    RAD_RT_ATTACHMENT_STENCIL = 0x0001,
    RAD_RT_ATTACHMENT_COLOR0 = 0x0002,
    RAD_RT_ATTACHMENT_COLOR1 = 0x0003,
    RAD_RT_ATTACHMENT_COLOR2 = 0x0004,
    RAD_RT_ATTACHMENT_COLOR3 = 0x0005,
    RAD_RT_ATTACHMENT_COLOR4 = 0x0006,
    RAD_RT_ATTACHMENT_COLOR5 = 0x0007,
    RAD_RT_ATTACHMENT_COLOR6 = 0x0008,
    RAD_RT_ATTACHMENT_COLOR7 = 0x0009,
} RADrtAttachment;

typedef enum {
    RAD_SYNC_ALL_GPU_COMMANDS_COMPLETE = 0x0000,
    RAD_SYNC_GRAPHICS_WORLD_SPACE_COMPLETE = 0x0001,
} RADsyncCondition;

typedef enum {
    RAD_WAIT_SYNC_ALREADY_SIGNALED = 0x0000,
    RAD_WAIT_SYNC_CONDITION_SATISFIED = 0x0001,
    RAD_WAIT_SYNC_TIMEOUT_EXPIRED = 0x0002,
    RAD_WAIT_SYNC_FAILED = 0x0003,
} RADwaitSyncResult;

typedef enum {
    RAD_TOKEN_NOP = 0x0000,
    RAD_TOKEN_BIND_GRAPHICS_PIPELINE = 0x0001,
    RAD_TOKEN_BIND_GROUP = 0x0002,
    RAD_TOKEN_DRAW_ARRAYS = 0x0003,
    RAD_TOKEN_DRAW_ELEMENTS = 0x0004,
    RAD_TOKEN_STENCIL_VALUE_MASK = 0x0005,
    RAD_TOKEN_STENCIL_MASK = 0x0006,
    RAD_TOKEN_STENCIL_REF = 0x0007,
    RAD_TOKEN_BLEND_COLOR = 0x0008,
    RAD_TOKEN_POINT_SIZE = 0x0009,
    RAD_TOKEN_LINE_WIDTH = 0x000A,
    RAD_TOKEN_POLYGON_OFFSET_CLAMP = 0x000B,
    RAD_TOKEN_SAMPLE_MASK = 0x000C,
} RADtokenName;

// XXX TODO: May need some ifdefs to make this pragma more portable
#pragma pack(push, 4)

typedef struct RADtokenBindGraphicsPipeline {
    RADuint64 pipelineHandle;
} RADtokenBindGraphicsPipeline;

typedef struct RADtokenBindGroup {
    RADuint stages;
    RADuint group;
    RADuint64 groupHandle;
    RADuint offset;
    RADuint count;
} RADtokenBindGroup;

typedef struct RADtokenDrawArrays {
    RADuint mode;
    RADuint first;
    RADuint count;
} RADtokenDrawArrays;

typedef struct RADtokenDrawElements {
    RADuint64 indexHandle;
    RADuint offset;
    RADuint mode;
    RADuint type;
    RADuint count;
} RADtokenDrawElements;

typedef struct RADtokenBlendColor {
    RADfloat color[4];
} RADtokenBlendColor;

typedef struct RADtokenStencilValueMask {
    RADuint faces;
    RADuint mask;
} RADtokenStencilValueMask;

typedef struct RADtokenStencilMask {
    RADuint faces;
    RADuint mask;
} RADtokenStencilMask;

typedef struct RADtokenStencilRef {
    RADuint faces;
    RADint ref;
} RADtokenStencilRef;

typedef struct RADtokenPointSize {
    RADfloat pointSize;
} RADtokenPointSize;

typedef struct RADtokenLineWidth {
    RADfloat lineWidth;
} RADtokenLineWidth;

typedef struct RADtokenPolygonOffsetClamp {
    RADfloat factor;
    RADfloat units;
    RADfloat clamp;
} RADtokenPolygonOffsetClamp;

typedef struct RADtokenSampleMask {
    RADuint mask;
} RADtokenSampleMask;

#pragma pack(pop)


#define RAD_FALSE                   0x0000
#define RAD_TRUE                    0x0001

#define RAD_VERTEX_ACCESS_BIT           0x00000001
#define RAD_UNIFORM_ACCESS_BIT          0x00000002
#define RAD_SHADER_STORAGE_ACCESS_BIT   0x00000004
#define RAD_INDEX_ACCESS_BIT            0x00000008
#define RAD_BINDGROUP_ACCESS_BIT        0x00000010
#define RAD_COPY_READ_ACCESS_BIT        0x00000020
#define RAD_COPY_WRITE_ACCESS_BIT       0x00000040
#define RAD_TEXTURE_ACCESS_BIT          0x00000080
#define RAD_RENDER_TARGET_ACCESS_BIT    0x00000100
#define RAD_IMAGE_ACCESS_BIT            0x00000200

#define RAD_MAP_READ_BIT                                     0x0001
#define RAD_MAP_WRITE_BIT                                    0x0002
#define RAD_MAP_PERSISTENT_BIT                               0x0040
#define RAD_MAP_COHERENT_BIT                                 0x0080


#define RAD_VERTEX_SHADER_BIT                                0x00000001
#define RAD_FRAGMENT_SHADER_BIT                              0x00000002
#define RAD_GEOMETRY_SHADER_BIT                              0x00000004
#define RAD_TESS_CONTROL_SHADER_BIT                          0x00000008
#define RAD_TESS_EVALUATION_SHADER_BIT                       0x00000010

#define RAD_SYNC_FLAG_FLUSH_FOR_CPU         0x00000001

#ifndef RAD_VERSION_1_0
#define RAD_VERSION_1_0 1
#ifdef RAD_PROTOTYPES
RADAPI RADPROC RADAPIENTRY radGetProcAddress (const RADchar *name);
RADAPI RADdevice RADAPIENTRY radCreateDevice (void);
RADAPI void RADAPIENTRY radReferenceDevice (RADdevice device);
RADAPI void RADAPIENTRY radReleaseDevice (RADdevice device);
RADAPI RADuint RADAPIENTRY radGetTokenHeader (RADdevice device, RADtokenName name);
RADAPI RADqueue RADAPIENTRY radCreateQueue (RADdevice device, RADqueueType queuetype);
RADAPI void RADAPIENTRY radReferenceQueue (RADqueue queue);
RADAPI void RADAPIENTRY radReleaseQueue (RADqueue queue);
RADAPI void RADAPIENTRY radQueueTagBuffer (RADqueue queue, RADbuffer buffer);
RADAPI void RADAPIENTRY radQueueTagTexture (RADqueue queue, RADtexture texture);
RADAPI void RADAPIENTRY radQueueSubmitCommands (RADqueue queue, RADuint numCommands, const RADcommandHandle *handles);
RADAPI void RADAPIENTRY radFlushQueue (RADqueue queue);
RADAPI void RADAPIENTRY radFinishQueue (RADqueue queue);
RADAPI void RADAPIENTRY radQueueViewport (RADqueue queue, RADint x, RADint y, RADint w, RADint h);
RADAPI void RADAPIENTRY radQueueScissor (RADqueue queue, RADint x, RADint y, RADint w, RADint h);
RADAPI void RADAPIENTRY radQueueCopyBufferToImage (RADqueue queue, RADbuffer buffer, RADintptr bufferOffset, RADtexture texture, RADint level, RADuint xoffset, RADuint yoffset, RADuint zoffset, RADsizei width, RADsizei height, RADsizei depth);
RADAPI void RADAPIENTRY radQueueCopyImageToBuffer (RADqueue queue, RADbuffer buffer, RADintptr bufferOffset, RADtexture texture, RADint level, RADuint xoffset, RADuint yoffset, RADuint zoffset, RADsizei width, RADsizei height, RADsizei depth);
RADAPI void RADAPIENTRY radQueueCopyBuffer (RADqueue queue, RADbuffer srcBuffer, RADintptr srcOffset, RADbuffer dstBuffer, RADintptr dstOffset, RADsizei size);
RADAPI void RADAPIENTRY radQueueClearColor (RADqueue queue, RADuint index, const RADfloat *color);
RADAPI void RADAPIENTRY radQueueClearDepth (RADqueue queue, RADfloat depth);
RADAPI void RADAPIENTRY radQueueClearStencil (RADqueue queue, RADuint stencil);
RADAPI void RADAPIENTRY radQueuePresent (RADqueue queue, RADtexture texture);
RADAPI void RADAPIENTRY radQueueDrawArrays (RADqueue queue, RADprimitiveType mode, RADint first, RADsizei count);
RADAPI void RADAPIENTRY radQueueDrawElements (RADqueue queue, RADprimitiveType mode, RADindexType type, RADsizei count, RADindexHandle indexHandle, RADuint offset);
RADAPI void RADAPIENTRY radQueueBindPipeline (RADqueue queue, RADpipelineType pipelineType, RADpipelineHandle pipelineHandle);
RADAPI void RADAPIENTRY radQueueBindGroup (RADqueue queue, RADbitfield stages, RADuint group, RADuint count, RADbindGroupHandle groupHandle, RADuint offset);
RADAPI void RADAPIENTRY radQueueBeginPass (RADqueue queue, RADpass pass);
RADAPI void RADAPIENTRY radQueueEndPass (RADqueue queue, RADpass pass);
RADAPI void RADAPIENTRY radQueueSubmitDynamic (RADqueue queue, const void *dynamic, RADsizei length);
RADAPI void RADAPIENTRY radQueueStencilValueMask (RADqueue queue, RADfaceBitfield faces, RADuint mask);
RADAPI void RADAPIENTRY radQueueStencilMask (RADqueue queue, RADfaceBitfield faces, RADuint mask);
RADAPI void RADAPIENTRY radQueueStencilRef (RADqueue queue, RADfaceBitfield faces, RADint ref);
RADAPI void RADAPIENTRY radQueueBlendColor (RADqueue queue, const RADfloat *blendColor);
RADAPI void RADAPIENTRY radQueuePointSize (RADqueue queue, RADfloat pointSize);
RADAPI void RADAPIENTRY radQueueLineWidth (RADqueue queue, RADfloat lineWidth);
RADAPI void RADAPIENTRY radQueuePolygonOffsetClamp (RADqueue queue, RADfloat factor, RADfloat units, RADfloat clamp);
RADAPI void RADAPIENTRY radQueueSampleMask (RADqueue queue, RADuint mask);
RADAPI RADprogram RADAPIENTRY radCreateProgram (RADdevice device);
RADAPI void RADAPIENTRY radReferenceProgram (RADprogram program);
RADAPI void RADAPIENTRY radReleaseProgram (RADprogram program);
RADAPI void RADAPIENTRY radProgramSource (RADprogram program, RADprogramFormat format, RADsizei length, const void *source);
RADAPI RADbuffer RADAPIENTRY radCreateBuffer (RADdevice device);
RADAPI void RADAPIENTRY radReferenceBuffer (RADbuffer buffer);
RADAPI void RADAPIENTRY radReleaseBuffer (RADbuffer buffer, RADtagMode tagMode);
RADAPI void RADAPIENTRY radBufferAccess (RADbuffer buffer, RADbitfield access);
RADAPI void RADAPIENTRY radBufferMapAccess (RADbuffer buffer, RADbitfield mapAccess);
RADAPI void RADAPIENTRY radBufferStorage (RADbuffer buffer, RADsizei size);
RADAPI void* RADAPIENTRY radMapBuffer (RADbuffer buffer);
RADAPI RADvertexHandle RADAPIENTRY radGetVertexHandle (RADbuffer buffer);
RADAPI RADindexHandle RADAPIENTRY radGetIndexHandle (RADbuffer buffer);
RADAPI RADuniformHandle RADAPIENTRY radGetUniformHandle (RADbuffer buffer);
RADAPI RADbindGroupHandle RADAPIENTRY radGetBindGroupHandle (RADbuffer buffer);
RADAPI RADtexture RADAPIENTRY radCreateTexture (RADdevice device);
RADAPI void RADAPIENTRY radReferenceTexture (RADtexture texture);
RADAPI void RADAPIENTRY radReleaseTexture (RADtexture texture, RADtagMode tagMode);
RADAPI void RADAPIENTRY radTextureAccess (RADtexture texture, RADbitfield access);
RADAPI void RADAPIENTRY radTextureStorage (RADtexture texture, RADtextureTarget target, RADsizei levels, RADinternalFormat internalFormat, RADsizei width, RADsizei height, RADsizei depth, RADsizei samples);
RADAPI RADtextureHandle RADAPIENTRY radGetTextureSamplerHandle (RADtexture texture, RADsampler sampler, RADtextureTarget target, RADinternalFormat internalFormat, RADuint minLevel, RADuint numLevels, RADuint minLayer, RADuint numLayers);
RADAPI RADrenderTargetHandle RADAPIENTRY radGetTextureRenderTargetHandle (RADtexture texture, RADtextureTarget target, RADinternalFormat internalFormat, RADuint level, RADuint minLayer, RADuint numLayers);
RADAPI RADsampler RADAPIENTRY radCreateSampler (RADdevice device);
RADAPI void RADAPIENTRY radReferenceSampler (RADsampler sampler);
RADAPI void RADAPIENTRY radReleaseSampler (RADsampler sampler);
RADAPI void RADAPIENTRY radSamplerDefault (RADsampler sampler);
RADAPI void RADAPIENTRY radSamplerMinMagFilter (RADsampler sampler, RADminFilter min, RADmagFilter mag);
RADAPI void RADAPIENTRY radSamplerWrapMode (RADsampler sampler, RADwrapMode s, RADwrapMode t, RADwrapMode r);
RADAPI void RADAPIENTRY radSamplerLodClamp (RADsampler sampler, RADfloat min, RADfloat max);
RADAPI void RADAPIENTRY radSamplerLodBias (RADsampler sampler, RADfloat bias);
RADAPI void RADAPIENTRY radSamplerCompare (RADsampler sampler, RADcompareMode mode, RADcompareFunc func);
RADAPI void RADAPIENTRY radSamplerBorderColorFloat (RADsampler sampler, const RADfloat *borderColor);
RADAPI void RADAPIENTRY radSamplerBorderColorInt (RADsampler sampler, const RADuint *borderColor);
RADAPI RADcolorState RADAPIENTRY radCreateColorState (RADdevice device);
RADAPI void RADAPIENTRY radReferenceColorState (RADcolorState color);
RADAPI void RADAPIENTRY radReleaseColorState (RADcolorState color);
RADAPI void RADAPIENTRY radColorDefault (RADcolorState color);
RADAPI void RADAPIENTRY radColorBlendEnable (RADcolorState color, RADuint index, RADboolean enable);
RADAPI void RADAPIENTRY radColorBlendFunc (RADcolorState color, RADuint index, RADblendFunc srcFunc, RADblendFunc dstFunc, RADblendFunc srcFuncAlpha, RADblendFunc dstFuncAlpha);
RADAPI void RADAPIENTRY radColorBlendEquation (RADcolorState color, RADuint index, RADblendEquation modeRGB, RADblendEquation modeAlpha);
RADAPI void RADAPIENTRY radColorMask (RADcolorState color, RADuint index, RADboolean r, RADboolean g, RADboolean b, RADboolean a);
RADAPI void RADAPIENTRY radColorNumTargets (RADcolorState color, RADuint numTargets);
RADAPI void RADAPIENTRY radColorLogicOpEnable (RADcolorState color, RADboolean enable);
RADAPI void RADAPIENTRY radColorLogicOp (RADcolorState color, RADlogicOp logicOp);
RADAPI void RADAPIENTRY radColorAlphaToCoverageEnable (RADcolorState color, RADboolean enable);
RADAPI void RADAPIENTRY radColorBlendColor (RADcolorState color, const RADfloat *blendColor);
RADAPI void RADAPIENTRY radColorDynamic (RADcolorState color, RADcolorDynamic dynamic, RADboolean enable);
RADAPI RADrasterState RADAPIENTRY radCreateRasterState (RADdevice device);
RADAPI void RADAPIENTRY radReferenceRasterState (RADrasterState raster);
RADAPI void RADAPIENTRY radReleaseRasterState (RADrasterState raster);
RADAPI void RADAPIENTRY radRasterDefault (RADrasterState raster);
RADAPI void RADAPIENTRY radRasterPointSize (RADrasterState raster, RADfloat pointSize);
RADAPI void RADAPIENTRY radRasterLineWidth (RADrasterState raster, RADfloat lineWidth);
RADAPI void RADAPIENTRY radRasterCullFace (RADrasterState raster, RADfaceBitfield face);
RADAPI void RADAPIENTRY radRasterFrontFace (RADrasterState raster, RADfrontFace face);
RADAPI void RADAPIENTRY radRasterPolygonMode (RADrasterState raster, RADpolygonMode polygonMode);
RADAPI void RADAPIENTRY radRasterPolygonOffsetClamp (RADrasterState raster, RADfloat factor, RADfloat units, RADfloat clamp);
RADAPI void RADAPIENTRY radRasterPolygonOffsetEnables (RADrasterState raster, RADpolygonOffsetEnables enables);
RADAPI void RADAPIENTRY radRasterDiscardEnable (RADrasterState raster, RADboolean enable);
RADAPI void RADAPIENTRY radRasterMultisampleEnable (RADrasterState raster, RADboolean enable);
RADAPI void RADAPIENTRY radRasterSamples (RADrasterState raster, RADuint samples);
RADAPI void RADAPIENTRY radRasterSampleMask (RADrasterState raster, RADuint mask);
RADAPI void RADAPIENTRY radRasterDynamic (RADrasterState raster, RADrasterDynamic dynamic, RADboolean enable);
RADAPI RADdepthStencilState RADAPIENTRY radCreateDepthStencilState (RADdevice device);
RADAPI void RADAPIENTRY radReferenceDepthStencilState (RADdepthStencilState depthStencil);
RADAPI void RADAPIENTRY radReleaseDepthStencilState (RADdepthStencilState depthStencil);
RADAPI void RADAPIENTRY radDepthStencilDefault (RADdepthStencilState depthStencil);
RADAPI void RADAPIENTRY radDepthStencilDepthTestEnable (RADdepthStencilState depthStencil, RADboolean enable);
RADAPI void RADAPIENTRY radDepthStencilDepthWriteEnable (RADdepthStencilState depthStencil, RADboolean enable);
RADAPI void RADAPIENTRY radDepthStencilDepthFunc (RADdepthStencilState depthStencil, RADdepthFunc func);
RADAPI void RADAPIENTRY radDepthStencilStencilTestEnable (RADdepthStencilState depthStencil, RADboolean enable);
RADAPI void RADAPIENTRY radDepthStencilStencilFunc (RADdepthStencilState depthStencil, RADfaceBitfield faces, RADstencilFunc func, RADint ref, RADuint mask);
RADAPI void RADAPIENTRY radDepthStencilStencilOp (RADdepthStencilState depthStencil, RADfaceBitfield faces, RADstencilOp fail, RADstencilOp depthFail, RADstencilOp depthPass);
RADAPI void RADAPIENTRY radDepthStencilStencilMask (RADdepthStencilState depthStencil, RADfaceBitfield faces, RADuint mask);
RADAPI void RADAPIENTRY radDepthStencilDynamic (RADdepthStencilState depthStencil, RADdepthStencilDynamic dynamic, RADboolean enable);
RADAPI RADvertexState RADAPIENTRY radCreateVertexState (RADdevice device);
RADAPI void RADAPIENTRY radReferenceVertexState (RADvertexState vertex);
RADAPI void RADAPIENTRY radReleaseVertexState (RADvertexState vertex);
RADAPI void RADAPIENTRY radVertexDefault (RADvertexState vertex);
RADAPI void RADAPIENTRY radVertexAttribFormat (RADvertexState vertex, RADint attribIndex, RADint numComponents, RADint bytesPerComponent, RADattribType type, RADuint relativeOffset);
RADAPI void RADAPIENTRY radVertexAttribBinding (RADvertexState vertex, RADint attribIndex, RADint bindingIndex);
RADAPI void RADAPIENTRY radVertexBindingGroup (RADvertexState vertex, RADint bindingIndex, RADint group, RADint index);
RADAPI void RADAPIENTRY radVertexAttribEnable (RADvertexState vertex, RADint attribIndex, RADboolean enable);
RADAPI void RADAPIENTRY radVertexBindingStride (RADvertexState vertex, RADint bindingIndex, RADuint stride);
RADAPI RADrtFormatState RADAPIENTRY radCreateRtFormatState (RADdevice device);
RADAPI void RADAPIENTRY radReferenceRtFormatState (RADrtFormatState rtFormat);
RADAPI void RADAPIENTRY radReleaseRtFormatState (RADrtFormatState rtFormat);
RADAPI void RADAPIENTRY radRtFormatDefault (RADrtFormatState rtFormat);
RADAPI void RADAPIENTRY radRtFormatColorFormat (RADrtFormatState rtFormat, RADuint index, RADinternalFormat format);
RADAPI void RADAPIENTRY radRtFormatDepthFormat (RADrtFormatState rtFormat, RADinternalFormat format);
RADAPI void RADAPIENTRY radRtFormatStencilFormat (RADrtFormatState rtFormat, RADinternalFormat format);
RADAPI void RADAPIENTRY radRtFormatColorSamples (RADrtFormatState rtFormat, RADuint samples);
RADAPI void RADAPIENTRY radRtFormatDepthStencilSamples (RADrtFormatState rtFormat, RADuint samples);
RADAPI RADpipeline RADAPIENTRY radCreatePipeline (RADdevice device, RADpipelineType pipelineType);
RADAPI void RADAPIENTRY radReferencePipeline (RADpipeline pipeline);
RADAPI void RADAPIENTRY radReleasePipeline (RADpipeline pipeline);
RADAPI void RADAPIENTRY radPipelineProgramStages (RADpipeline pipeline, RADbitfield stages, RADprogram program);
RADAPI void RADAPIENTRY radPipelineVertexState (RADpipeline pipeline, RADvertexState vertex);
RADAPI void RADAPIENTRY radPipelineColorState (RADpipeline pipeline, RADcolorState color);
RADAPI void RADAPIENTRY radPipelineRasterState (RADpipeline pipeline, RADrasterState raster);
RADAPI void RADAPIENTRY radPipelineDepthStencilState (RADpipeline pipeline, RADdepthStencilState depthStencil);
RADAPI void RADAPIENTRY radPipelineRtFormatState (RADpipeline pipeline, RADrtFormatState rtFormat);
RADAPI void RADAPIENTRY radPipelinePrimitiveType (RADpipeline pipeline, RADprimitiveType mode);
RADAPI void RADAPIENTRY radCompilePipeline (RADpipeline pipeline);
RADAPI RADpipelineHandle RADAPIENTRY radGetPipelineHandle (RADpipeline pipeline);
RADAPI RADcommandBuffer RADAPIENTRY radCreateCommandBuffer (RADdevice device, RADqueueType queueType);
RADAPI void RADAPIENTRY radReferenceCommandBuffer (RADcommandBuffer cmdBuf);
RADAPI void RADAPIENTRY radReleaseCommandBuffer (RADcommandBuffer cmdBuf);
RADAPI void RADAPIENTRY radCmdBindPipeline (RADcommandBuffer cmdBuf, RADpipelineType pipelineType, RADpipelineHandle pipelineHandle);
RADAPI void RADAPIENTRY radCmdBindGroup (RADcommandBuffer cmdBuf, RADbitfield stages, RADuint group, RADuint count, RADbindGroupHandle groupHandle, RADuint offset);
RADAPI void RADAPIENTRY radCmdDrawArrays (RADcommandBuffer cmdBuf, RADprimitiveType mode, RADint first, RADsizei count);
RADAPI void RADAPIENTRY radCmdDrawElements (RADcommandBuffer cmdBuf, RADprimitiveType mode, RADindexType type, RADsizei count, RADindexHandle indexHandle, RADuint offset);
RADAPI RADboolean RADAPIENTRY radCompileCommandBuffer (RADcommandBuffer cmdBuf);
RADAPI RADcommandHandle RADAPIENTRY radGetCommandHandle (RADcommandBuffer cmdBuf);
RADAPI void RADAPIENTRY radCmdStencilValueMask (RADcommandBuffer cmdBuf, RADfaceBitfield faces, RADuint mask);
RADAPI void RADAPIENTRY radCmdStencilMask (RADcommandBuffer cmdBuf, RADfaceBitfield faces, RADuint mask);
RADAPI void RADAPIENTRY radCmdStencilRef (RADcommandBuffer cmdBuf, RADfaceBitfield faces, RADint ref);
RADAPI void RADAPIENTRY radCmdBlendColor (RADcommandBuffer cmdBuf, const RADfloat *blendColor);
RADAPI void RADAPIENTRY radCmdPointSize (RADcommandBuffer cmdBuf, RADfloat pointSize);
RADAPI void RADAPIENTRY radCmdLineWidth (RADcommandBuffer cmdBuf, RADfloat lineWidth);
RADAPI void RADAPIENTRY radCmdPolygonOffsetClamp (RADcommandBuffer cmdBuf, RADfloat factor, RADfloat units, RADfloat clamp);
RADAPI void RADAPIENTRY radCmdSampleMask (RADcommandBuffer cmdBuf, RADuint mask);
RADAPI RADpass RADAPIENTRY radCreatePass (RADdevice device);
RADAPI void RADAPIENTRY radReferencePass (RADpass pass);
RADAPI void RADAPIENTRY radReleasePass (RADpass pass);
RADAPI void RADAPIENTRY radPassDefault (RADpass pass);
RADAPI void RADAPIENTRY radCompilePass (RADpass pass);
RADAPI void RADAPIENTRY radPassRenderTargets (RADpass pass, RADuint numColors, const RADrenderTargetHandle *colors, RADrenderTargetHandle depth, RADrenderTargetHandle stencil);
RADAPI void RADAPIENTRY radPassPreserveEnable (RADpass pass, RADrtAttachment attachment, RADboolean enable);
RADAPI void RADAPIENTRY radPassDiscard (RADpass pass, RADuint numTextures, const RADtexture *textures, const RADoffset2D *offsets);
RADAPI void RADAPIENTRY radPassResolve (RADpass pass, RADrtAttachment attachment, RADtexture texture);
RADAPI void RADAPIENTRY radPassStore (RADpass pass, RADuint numTextures, const RADtexture *textures, const RADoffset2D *offsets);
RADAPI void RADAPIENTRY radPassClip (RADpass pass, const RADrect2D *rect);
RADAPI void RADAPIENTRY radPassDependencies (RADpass pass, RADuint numPasses, const RADpass *otherPasses, const RADbitfield *srcMask, const RADbitfield *dstMask, const RADbitfield *flushMask, const RADbitfield *invalidateMask);
RADAPI void RADAPIENTRY radPassTilingBoundary (RADpass pass, RADboolean boundary);
RADAPI void RADAPIENTRY radPassTileFilterWidth (RADpass pass, RADuint filterWidth, RADuint filterHeight);
RADAPI void RADAPIENTRY radPassTileFootprint (RADpass pass, RADuint bytesPerPixel, RADuint maxFilterWidth, RADuint maxFilterHeight);
RADAPI RADsync RADAPIENTRY radCreateSync (RADdevice device);
RADAPI void RADAPIENTRY radReferenceSync (RADsync sync);
RADAPI void RADAPIENTRY radReleaseSync (RADsync sync);
RADAPI void RADAPIENTRY radQueueFenceSync (RADqueue queue, RADsync sync, RADsyncCondition condition, RADbitfield flags);
RADAPI RADwaitSyncResult RADAPIENTRY radWaitSync (RADsync sync, RADuint64 timeout);
RADAPI RADboolean RADAPIENTRY radQueueWaitSync (RADqueue queue, RADsync sync);
#endif /* RAD_PROTOTYPES */
typedef RADPROC (RADAPIENTRYP PFNRADGETPROCADDRESSPROC) (const RADchar *name);
typedef RADdevice (RADAPIENTRYP PFNRADCREATEDEVICEPROC) (void);
typedef void (RADAPIENTRYP PFNRADREFERENCEDEVICEPROC) (RADdevice device);
typedef void (RADAPIENTRYP PFNRADRELEASEDEVICEPROC) (RADdevice device);
typedef RADuint (RADAPIENTRYP PFNRADGETTOKENHEADERPROC) (RADdevice device, RADtokenName name);
typedef RADqueue (RADAPIENTRYP PFNRADCREATEQUEUEPROC) (RADdevice device, RADqueueType queuetype);
typedef void (RADAPIENTRYP PFNRADREFERENCEQUEUEPROC) (RADqueue queue);
typedef void (RADAPIENTRYP PFNRADRELEASEQUEUEPROC) (RADqueue queue);
typedef void (RADAPIENTRYP PFNRADQUEUETAGBUFFERPROC) (RADqueue queue, RADbuffer buffer);
typedef void (RADAPIENTRYP PFNRADQUEUETAGTEXTUREPROC) (RADqueue queue, RADtexture texture);
typedef void (RADAPIENTRYP PFNRADQUEUESUBMITCOMMANDSPROC) (RADqueue queue, RADuint numCommands, const RADcommandHandle *handles);
typedef void (RADAPIENTRYP PFNRADFLUSHQUEUEPROC) (RADqueue queue);
typedef void (RADAPIENTRYP PFNRADFINISHQUEUEPROC) (RADqueue queue);
typedef void (RADAPIENTRYP PFNRADQUEUEVIEWPORTPROC) (RADqueue queue, RADint x, RADint y, RADint w, RADint h);
typedef void (RADAPIENTRYP PFNRADQUEUESCISSORPROC) (RADqueue queue, RADint x, RADint y, RADint w, RADint h);
typedef void (RADAPIENTRYP PFNRADQUEUECOPYBUFFERTOIMAGEPROC) (RADqueue queue, RADbuffer buffer, RADintptr bufferOffset, RADtexture texture, RADint level, RADuint xoffset, RADuint yoffset, RADuint zoffset, RADsizei width, RADsizei height, RADsizei depth);
typedef void (RADAPIENTRYP PFNRADQUEUECOPYIMAGETOBUFFERPROC) (RADqueue queue, RADbuffer buffer, RADintptr bufferOffset, RADtexture texture, RADint level, RADuint xoffset, RADuint yoffset, RADuint zoffset, RADsizei width, RADsizei height, RADsizei depth);
typedef void (RADAPIENTRYP PFNRADQUEUECOPYBUFFERPROC) (RADqueue queue, RADbuffer srcBuffer, RADintptr srcOffset, RADbuffer dstBuffer, RADintptr dstOffset, RADsizei size);
typedef void (RADAPIENTRYP PFNRADQUEUECLEARCOLORPROC) (RADqueue queue, RADuint index, const RADfloat *color);
typedef void (RADAPIENTRYP PFNRADQUEUECLEARDEPTHPROC) (RADqueue queue, RADfloat depth);
typedef void (RADAPIENTRYP PFNRADQUEUECLEARSTENCILPROC) (RADqueue queue, RADuint stencil);
typedef void (RADAPIENTRYP PFNRADQUEUEPRESENTPROC) (RADqueue queue, RADtexture texture);
typedef void (RADAPIENTRYP PFNRADQUEUEDRAWARRAYSPROC) (RADqueue queue, RADprimitiveType mode, RADint first, RADsizei count);
typedef void (RADAPIENTRYP PFNRADQUEUEDRAWELEMENTSPROC) (RADqueue queue, RADprimitiveType mode, RADindexType type, RADsizei count, RADindexHandle indexHandle, RADuint offset);
typedef void (RADAPIENTRYP PFNRADQUEUEBINDPIPELINEPROC) (RADqueue queue, RADpipelineType pipelineType, RADpipelineHandle pipelineHandle);
typedef void (RADAPIENTRYP PFNRADQUEUEBINDGROUPPROC) (RADqueue queue, RADbitfield stages, RADuint group, RADuint count, RADbindGroupHandle groupHandle, RADuint offset);
typedef void (RADAPIENTRYP PFNRADQUEUEBEGINPASSPROC) (RADqueue queue, RADpass pass);
typedef void (RADAPIENTRYP PFNRADQUEUEENDPASSPROC) (RADqueue queue, RADpass pass);
typedef void (RADAPIENTRYP PFNRADQUEUESUBMITDYNAMICPROC) (RADqueue queue, const void *dynamic, RADsizei length);
typedef void (RADAPIENTRYP PFNRADQUEUESTENCILVALUEMASKPROC) (RADqueue queue, RADfaceBitfield faces, RADuint mask);
typedef void (RADAPIENTRYP PFNRADQUEUESTENCILMASKPROC) (RADqueue queue, RADfaceBitfield faces, RADuint mask);
typedef void (RADAPIENTRYP PFNRADQUEUESTENCILREFPROC) (RADqueue queue, RADfaceBitfield faces, RADint ref);
typedef void (RADAPIENTRYP PFNRADQUEUEBLENDCOLORPROC) (RADqueue queue, const RADfloat *blendColor);
typedef void (RADAPIENTRYP PFNRADQUEUEPOINTSIZEPROC) (RADqueue queue, RADfloat pointSize);
typedef void (RADAPIENTRYP PFNRADQUEUELINEWIDTHPROC) (RADqueue queue, RADfloat lineWidth);
typedef void (RADAPIENTRYP PFNRADQUEUEPOLYGONOFFSETCLAMPPROC) (RADqueue queue, RADfloat factor, RADfloat units, RADfloat clamp);
typedef void (RADAPIENTRYP PFNRADQUEUESAMPLEMASKPROC) (RADqueue queue, RADuint mask);
typedef RADprogram (RADAPIENTRYP PFNRADCREATEPROGRAMPROC) (RADdevice device);
typedef void (RADAPIENTRYP PFNRADREFERENCEPROGRAMPROC) (RADprogram program);
typedef void (RADAPIENTRYP PFNRADRELEASEPROGRAMPROC) (RADprogram program);
typedef void (RADAPIENTRYP PFNRADPROGRAMSOURCEPROC) (RADprogram program, RADprogramFormat format, RADsizei length, const void *source);
typedef RADbuffer (RADAPIENTRYP PFNRADCREATEBUFFERPROC) (RADdevice device);
typedef void (RADAPIENTRYP PFNRADREFERENCEBUFFERPROC) (RADbuffer buffer);
typedef void (RADAPIENTRYP PFNRADRELEASEBUFFERPROC) (RADbuffer buffer, RADtagMode tagMode);
typedef void (RADAPIENTRYP PFNRADBUFFERACCESSPROC) (RADbuffer buffer, RADbitfield access);
typedef void (RADAPIENTRYP PFNRADBUFFERMAPACCESSPROC) (RADbuffer buffer, RADbitfield mapAccess);
typedef void (RADAPIENTRYP PFNRADBUFFERSTORAGEPROC) (RADbuffer buffer, RADsizei size);
typedef void* (RADAPIENTRYP PFNRADMAPBUFFERPROC) (RADbuffer buffer);
typedef RADvertexHandle (RADAPIENTRYP PFNRADGETVERTEXHANDLEPROC) (RADbuffer buffer);
typedef RADindexHandle (RADAPIENTRYP PFNRADGETINDEXHANDLEPROC) (RADbuffer buffer);
typedef RADuniformHandle (RADAPIENTRYP PFNRADGETUNIFORMHANDLEPROC) (RADbuffer buffer);
typedef RADbindGroupHandle (RADAPIENTRYP PFNRADGETBINDGROUPHANDLEPROC) (RADbuffer buffer);
typedef RADtexture (RADAPIENTRYP PFNRADCREATETEXTUREPROC) (RADdevice device);
typedef void (RADAPIENTRYP PFNRADREFERENCETEXTUREPROC) (RADtexture texture);
typedef void (RADAPIENTRYP PFNRADRELEASETEXTUREPROC) (RADtexture texture, RADtagMode tagMode);
typedef void (RADAPIENTRYP PFNRADTEXTUREACCESSPROC) (RADtexture texture, RADbitfield access);
typedef void (RADAPIENTRYP PFNRADTEXTURESTORAGEPROC) (RADtexture texture, RADtextureTarget target, RADsizei levels, RADinternalFormat internalFormat, RADsizei width, RADsizei height, RADsizei depth, RADsizei samples);
typedef RADtextureHandle (RADAPIENTRYP PFNRADGETTEXTURESAMPLERHANDLEPROC) (RADtexture texture, RADsampler sampler, RADtextureTarget target, RADinternalFormat internalFormat, RADuint minLevel, RADuint numLevels, RADuint minLayer, RADuint numLayers);
typedef RADrenderTargetHandle (RADAPIENTRYP PFNRADGETTEXTURERENDERTARGETHANDLEPROC) (RADtexture texture, RADtextureTarget target, RADinternalFormat internalFormat, RADuint level, RADuint minLayer, RADuint numLayers);
typedef RADsampler (RADAPIENTRYP PFNRADCREATESAMPLERPROC) (RADdevice device);
typedef void (RADAPIENTRYP PFNRADREFERENCESAMPLERPROC) (RADsampler sampler);
typedef void (RADAPIENTRYP PFNRADRELEASESAMPLERPROC) (RADsampler sampler);
typedef void (RADAPIENTRYP PFNRADSAMPLERDEFAULTPROC) (RADsampler sampler);
typedef void (RADAPIENTRYP PFNRADSAMPLERMINMAGFILTERPROC) (RADsampler sampler, RADminFilter min, RADmagFilter mag);
typedef void (RADAPIENTRYP PFNRADSAMPLERWRAPMODEPROC) (RADsampler sampler, RADwrapMode s, RADwrapMode t, RADwrapMode r);
typedef void (RADAPIENTRYP PFNRADSAMPLERLODCLAMPPROC) (RADsampler sampler, RADfloat min, RADfloat max);
typedef void (RADAPIENTRYP PFNRADSAMPLERLODBIASPROC) (RADsampler sampler, RADfloat bias);
typedef void (RADAPIENTRYP PFNRADSAMPLERCOMPAREPROC) (RADsampler sampler, RADcompareMode mode, RADcompareFunc func);
typedef void (RADAPIENTRYP PFNRADSAMPLERBORDERCOLORFLOATPROC) (RADsampler sampler, const RADfloat *borderColor);
typedef void (RADAPIENTRYP PFNRADSAMPLERBORDERCOLORINTPROC) (RADsampler sampler, const RADuint *borderColor);
typedef RADcolorState (RADAPIENTRYP PFNRADCREATECOLORSTATEPROC) (RADdevice device);
typedef void (RADAPIENTRYP PFNRADREFERENCECOLORSTATEPROC) (RADcolorState color);
typedef void (RADAPIENTRYP PFNRADRELEASECOLORSTATEPROC) (RADcolorState color);
typedef void (RADAPIENTRYP PFNRADCOLORDEFAULTPROC) (RADcolorState color);
typedef void (RADAPIENTRYP PFNRADCOLORBLENDENABLEPROC) (RADcolorState color, RADuint index, RADboolean enable);
typedef void (RADAPIENTRYP PFNRADCOLORBLENDFUNCPROC) (RADcolorState color, RADuint index, RADblendFunc srcFunc, RADblendFunc dstFunc, RADblendFunc srcFuncAlpha, RADblendFunc dstFuncAlpha);
typedef void (RADAPIENTRYP PFNRADCOLORBLENDEQUATIONPROC) (RADcolorState color, RADuint index, RADblendEquation modeRGB, RADblendEquation modeAlpha);
typedef void (RADAPIENTRYP PFNRADCOLORMASKPROC) (RADcolorState color, RADuint index, RADboolean r, RADboolean g, RADboolean b, RADboolean a);
typedef void (RADAPIENTRYP PFNRADCOLORNUMTARGETSPROC) (RADcolorState color, RADuint numTargets);
typedef void (RADAPIENTRYP PFNRADCOLORLOGICOPENABLEPROC) (RADcolorState color, RADboolean enable);
typedef void (RADAPIENTRYP PFNRADCOLORLOGICOPPROC) (RADcolorState color, RADlogicOp logicOp);
typedef void (RADAPIENTRYP PFNRADCOLORALPHATOCOVERAGEENABLEPROC) (RADcolorState color, RADboolean enable);
typedef void (RADAPIENTRYP PFNRADCOLORBLENDCOLORPROC) (RADcolorState color, const RADfloat *blendColor);
typedef void (RADAPIENTRYP PFNRADCOLORDYNAMICPROC) (RADcolorState color, RADcolorDynamic dynamic, RADboolean enable);
typedef RADrasterState (RADAPIENTRYP PFNRADCREATERASTERSTATEPROC) (RADdevice device);
typedef void (RADAPIENTRYP PFNRADREFERENCERASTERSTATEPROC) (RADrasterState raster);
typedef void (RADAPIENTRYP PFNRADRELEASERASTERSTATEPROC) (RADrasterState raster);
typedef void (RADAPIENTRYP PFNRADRASTERDEFAULTPROC) (RADrasterState raster);
typedef void (RADAPIENTRYP PFNRADRASTERPOINTSIZEPROC) (RADrasterState raster, RADfloat pointSize);
typedef void (RADAPIENTRYP PFNRADRASTERLINEWIDTHPROC) (RADrasterState raster, RADfloat lineWidth);
typedef void (RADAPIENTRYP PFNRADRASTERCULLFACEPROC) (RADrasterState raster, RADfaceBitfield face);
typedef void (RADAPIENTRYP PFNRADRASTERFRONTFACEPROC) (RADrasterState raster, RADfrontFace face);
typedef void (RADAPIENTRYP PFNRADRASTERPOLYGONMODEPROC) (RADrasterState raster, RADpolygonMode polygonMode);
typedef void (RADAPIENTRYP PFNRADRASTERPOLYGONOFFSETCLAMPPROC) (RADrasterState raster, RADfloat factor, RADfloat units, RADfloat clamp);
typedef void (RADAPIENTRYP PFNRADRASTERPOLYGONOFFSETENABLESPROC) (RADrasterState raster, RADpolygonOffsetEnables enables);
typedef void (RADAPIENTRYP PFNRADRASTERDISCARDENABLEPROC) (RADrasterState raster, RADboolean enable);
typedef void (RADAPIENTRYP PFNRADRASTERMULTISAMPLEENABLEPROC) (RADrasterState raster, RADboolean enable);
typedef void (RADAPIENTRYP PFNRADRASTERSAMPLESPROC) (RADrasterState raster, RADuint samples);
typedef void (RADAPIENTRYP PFNRADRASTERSAMPLEMASKPROC) (RADrasterState raster, RADuint mask);
typedef void (RADAPIENTRYP PFNRADRASTERDYNAMICPROC) (RADrasterState raster, RADrasterDynamic dynamic, RADboolean enable);
typedef RADdepthStencilState (RADAPIENTRYP PFNRADCREATEDEPTHSTENCILSTATEPROC) (RADdevice device);
typedef void (RADAPIENTRYP PFNRADREFERENCEDEPTHSTENCILSTATEPROC) (RADdepthStencilState depthStencil);
typedef void (RADAPIENTRYP PFNRADRELEASEDEPTHSTENCILSTATEPROC) (RADdepthStencilState depthStencil);
typedef void (RADAPIENTRYP PFNRADDEPTHSTENCILDEFAULTPROC) (RADdepthStencilState depthStencil);
typedef void (RADAPIENTRYP PFNRADDEPTHSTENCILDEPTHTESTENABLEPROC) (RADdepthStencilState depthStencil, RADboolean enable);
typedef void (RADAPIENTRYP PFNRADDEPTHSTENCILDEPTHWRITEENABLEPROC) (RADdepthStencilState depthStencil, RADboolean enable);
typedef void (RADAPIENTRYP PFNRADDEPTHSTENCILDEPTHFUNCPROC) (RADdepthStencilState depthStencil, RADdepthFunc func);
typedef void (RADAPIENTRYP PFNRADDEPTHSTENCILSTENCILTESTENABLEPROC) (RADdepthStencilState depthStencil, RADboolean enable);
typedef void (RADAPIENTRYP PFNRADDEPTHSTENCILSTENCILFUNCPROC) (RADdepthStencilState depthStencil, RADfaceBitfield faces, RADstencilFunc func, RADint ref, RADuint mask);
typedef void (RADAPIENTRYP PFNRADDEPTHSTENCILSTENCILOPPROC) (RADdepthStencilState depthStencil, RADfaceBitfield faces, RADstencilOp fail, RADstencilOp depthFail, RADstencilOp depthPass);
typedef void (RADAPIENTRYP PFNRADDEPTHSTENCILSTENCILMASKPROC) (RADdepthStencilState depthStencil, RADfaceBitfield faces, RADuint mask);
typedef void (RADAPIENTRYP PFNRADDEPTHSTENCILDYNAMICPROC) (RADdepthStencilState depthStencil, RADdepthStencilDynamic dynamic, RADboolean enable);
typedef RADvertexState (RADAPIENTRYP PFNRADCREATEVERTEXSTATEPROC) (RADdevice device);
typedef void (RADAPIENTRYP PFNRADREFERENCEVERTEXSTATEPROC) (RADvertexState vertex);
typedef void (RADAPIENTRYP PFNRADRELEASEVERTEXSTATEPROC) (RADvertexState vertex);
typedef void (RADAPIENTRYP PFNRADVERTEXDEFAULTPROC) (RADvertexState vertex);
typedef void (RADAPIENTRYP PFNRADVERTEXATTRIBFORMATPROC) (RADvertexState vertex, RADint attribIndex, RADint numComponents, RADint bytesPerComponent, RADattribType type, RADuint relativeOffset);
typedef void (RADAPIENTRYP PFNRADVERTEXATTRIBBINDINGPROC) (RADvertexState vertex, RADint attribIndex, RADint bindingIndex);
typedef void (RADAPIENTRYP PFNRADVERTEXBINDINGGROUPPROC) (RADvertexState vertex, RADint bindingIndex, RADint group, RADint index);
typedef void (RADAPIENTRYP PFNRADVERTEXATTRIBENABLEPROC) (RADvertexState vertex, RADint attribIndex, RADboolean enable);
typedef void (RADAPIENTRYP PFNRADVERTEXBINDINGSTRIDEPROC) (RADvertexState vertex, RADint bindingIndex, RADuint stride);
typedef RADrtFormatState (RADAPIENTRYP PFNRADCREATERTFORMATSTATEPROC) (RADdevice device);
typedef void (RADAPIENTRYP PFNRADREFERENCERTFORMATSTATEPROC) (RADrtFormatState rtFormat);
typedef void (RADAPIENTRYP PFNRADRELEASERTFORMATSTATEPROC) (RADrtFormatState rtFormat);
typedef void (RADAPIENTRYP PFNRADRTFORMATDEFAULTPROC) (RADrtFormatState rtFormat);
typedef void (RADAPIENTRYP PFNRADRTFORMATCOLORFORMATPROC) (RADrtFormatState rtFormat, RADuint index, RADinternalFormat format);
typedef void (RADAPIENTRYP PFNRADRTFORMATDEPTHFORMATPROC) (RADrtFormatState rtFormat, RADinternalFormat format);
typedef void (RADAPIENTRYP PFNRADRTFORMATSTENCILFORMATPROC) (RADrtFormatState rtFormat, RADinternalFormat format);
typedef void (RADAPIENTRYP PFNRADRTFORMATCOLORSAMPLESPROC) (RADrtFormatState rtFormat, RADuint samples);
typedef void (RADAPIENTRYP PFNRADRTFORMATDEPTHSTENCILSAMPLESPROC) (RADrtFormatState rtFormat, RADuint samples);
typedef RADpipeline (RADAPIENTRYP PFNRADCREATEPIPELINEPROC) (RADdevice device, RADpipelineType pipelineType);
typedef void (RADAPIENTRYP PFNRADREFERENCEPIPELINEPROC) (RADpipeline pipeline);
typedef void (RADAPIENTRYP PFNRADRELEASEPIPELINEPROC) (RADpipeline pipeline);
typedef void (RADAPIENTRYP PFNRADPIPELINEPROGRAMSTAGESPROC) (RADpipeline pipeline, RADbitfield stages, RADprogram program);
typedef void (RADAPIENTRYP PFNRADPIPELINEVERTEXSTATEPROC) (RADpipeline pipeline, RADvertexState vertex);
typedef void (RADAPIENTRYP PFNRADPIPELINECOLORSTATEPROC) (RADpipeline pipeline, RADcolorState color);
typedef void (RADAPIENTRYP PFNRADPIPELINERASTERSTATEPROC) (RADpipeline pipeline, RADrasterState raster);
typedef void (RADAPIENTRYP PFNRADPIPELINEDEPTHSTENCILSTATEPROC) (RADpipeline pipeline, RADdepthStencilState depthStencil);
typedef void (RADAPIENTRYP PFNRADPIPELINERTFORMATSTATEPROC) (RADpipeline pipeline, RADrtFormatState rtFormat);
typedef void (RADAPIENTRYP PFNRADPIPELINEPRIMITIVETYPEPROC) (RADpipeline pipeline, RADprimitiveType mode);
typedef void (RADAPIENTRYP PFNRADCOMPILEPIPELINEPROC) (RADpipeline pipeline);
typedef RADpipelineHandle (RADAPIENTRYP PFNRADGETPIPELINEHANDLEPROC) (RADpipeline pipeline);
typedef RADcommandBuffer (RADAPIENTRYP PFNRADCREATECOMMANDBUFFERPROC) (RADdevice device, RADqueueType queueType);
typedef void (RADAPIENTRYP PFNRADREFERENCECOMMANDBUFFERPROC) (RADcommandBuffer cmdBuf);
typedef void (RADAPIENTRYP PFNRADRELEASECOMMANDBUFFERPROC) (RADcommandBuffer cmdBuf);
typedef void (RADAPIENTRYP PFNRADCMDBINDPIPELINEPROC) (RADcommandBuffer cmdBuf, RADpipelineType pipelineType, RADpipelineHandle pipelineHandle);
typedef void (RADAPIENTRYP PFNRADCMDBINDGROUPPROC) (RADcommandBuffer cmdBuf, RADbitfield stages, RADuint group, RADuint count, RADbindGroupHandle groupHandle, RADuint offset);
typedef void (RADAPIENTRYP PFNRADCMDDRAWARRAYSPROC) (RADcommandBuffer cmdBuf, RADprimitiveType mode, RADint first, RADsizei count);
typedef void (RADAPIENTRYP PFNRADCMDDRAWELEMENTSPROC) (RADcommandBuffer cmdBuf, RADprimitiveType mode, RADindexType type, RADsizei count, RADindexHandle indexHandle, RADuint offset);
typedef RADboolean (RADAPIENTRYP PFNRADCOMPILECOMMANDBUFFERPROC) (RADcommandBuffer cmdBuf);
typedef RADcommandHandle (RADAPIENTRYP PFNRADGETCOMMANDHANDLEPROC) (RADcommandBuffer cmdBuf);
typedef void (RADAPIENTRYP PFNRADCMDSTENCILVALUEMASKPROC) (RADcommandBuffer cmdBuf, RADfaceBitfield faces, RADuint mask);
typedef void (RADAPIENTRYP PFNRADCMDSTENCILMASKPROC) (RADcommandBuffer cmdBuf, RADfaceBitfield faces, RADuint mask);
typedef void (RADAPIENTRYP PFNRADCMDSTENCILREFPROC) (RADcommandBuffer cmdBuf, RADfaceBitfield faces, RADint ref);
typedef void (RADAPIENTRYP PFNRADCMDBLENDCOLORPROC) (RADcommandBuffer cmdBuf, const RADfloat *blendColor);
typedef void (RADAPIENTRYP PFNRADCMDPOINTSIZEPROC) (RADcommandBuffer cmdBuf, RADfloat pointSize);
typedef void (RADAPIENTRYP PFNRADCMDLINEWIDTHPROC) (RADcommandBuffer cmdBuf, RADfloat lineWidth);
typedef void (RADAPIENTRYP PFNRADCMDPOLYGONOFFSETCLAMPPROC) (RADcommandBuffer cmdBuf, RADfloat factor, RADfloat units, RADfloat clamp);
typedef void (RADAPIENTRYP PFNRADCMDSAMPLEMASKPROC) (RADcommandBuffer cmdBuf, RADuint mask);
typedef RADpass (RADAPIENTRYP PFNRADCREATEPASSPROC) (RADdevice device);
typedef void (RADAPIENTRYP PFNRADREFERENCEPASSPROC) (RADpass pass);
typedef void (RADAPIENTRYP PFNRADRELEASEPASSPROC) (RADpass pass);
typedef void (RADAPIENTRYP PFNRADPASSDEFAULTPROC) (RADpass pass);
typedef void (RADAPIENTRYP PFNRADCOMPILEPASSPROC) (RADpass pass);
typedef void (RADAPIENTRYP PFNRADPASSRENDERTARGETSPROC) (RADpass pass, RADuint numColors, const RADrenderTargetHandle *colors, RADrenderTargetHandle depth, RADrenderTargetHandle stencil);
typedef void (RADAPIENTRYP PFNRADPASSPRESERVEENABLEPROC) (RADpass pass, RADrtAttachment attachment, RADboolean enable);
typedef void (RADAPIENTRYP PFNRADPASSDISCARDPROC) (RADpass pass, RADuint numTextures, const RADtexture *textures, const RADoffset2D *offsets);
typedef void (RADAPIENTRYP PFNRADPASSRESOLVEPROC) (RADpass pass, RADrtAttachment attachment, RADtexture texture);
typedef void (RADAPIENTRYP PFNRADPASSSTOREPROC) (RADpass pass, RADuint numTextures, const RADtexture *textures, const RADoffset2D *offsets);
typedef void (RADAPIENTRYP PFNRADPASSCLIPPROC) (RADpass pass, const RADrect2D *rect);
typedef void (RADAPIENTRYP PFNRADPASSDEPENDENCIESPROC) (RADpass pass, RADuint numPasses, const RADpass *otherPasses, const RADbitfield *srcMask, const RADbitfield *dstMask, const RADbitfield *flushMask, const RADbitfield *invalidateMask);
typedef void (RADAPIENTRYP PFNRADPASSTILINGBOUNDARYPROC) (RADpass pass, RADboolean boundary);
typedef void (RADAPIENTRYP PFNRADPASSTILEFILTERWIDTHPROC) (RADpass pass, RADuint filterWidth, RADuint filterHeight);
typedef void (RADAPIENTRYP PFNRADPASSTILEFOOTPRINTPROC) (RADpass pass, RADuint bytesPerPixel, RADuint maxFilterWidth, RADuint maxFilterHeight);
typedef RADsync (RADAPIENTRYP PFNRADCREATESYNCPROC) (RADdevice device);
typedef void (RADAPIENTRYP PFNRADREFERENCESYNCPROC) (RADsync sync);
typedef void (RADAPIENTRYP PFNRADRELEASESYNCPROC) (RADsync sync);
typedef void (RADAPIENTRYP PFNRADQUEUEFENCESYNCPROC) (RADqueue queue, RADsync sync, RADsyncCondition condition, RADbitfield flags);
typedef RADwaitSyncResult (RADAPIENTRYP PFNRADWAITSYNCPROC) (RADsync sync, RADuint64 timeout);
typedef RADboolean (RADAPIENTRYP PFNRADQUEUEWAITSYNCPROC) (RADqueue queue, RADsync sync);
#endif



#endif /* __rad_h_ */
