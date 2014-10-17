
#include "stdlib.h"
#include "rad.h"
#include "radfnptrinit.h"
#include "string.h"
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __linux__
#include <GL/glx.h>
#endif
#include <stdio.h>
#include <time.h>

RADdevice device;
RADqueue queue;
bool useCopyQueue = true;
RADqueue copyQueue;

static const char *vsstring = 
    "#version 440 core\n"
    "#define BINDGROUP(GROUP,INDEX) layout(binding = ((INDEX) | ((GROUP) << 4)))\n"
    "layout(location = 1) in vec4 tc;\n"
    "layout(location = 0) in vec4 position;\n"
    "BINDGROUP(0, 2) uniform Block {\n"
    "    vec4 scale;\n"
    "};\n"
    "out vec4 ftc;\n"
    "void main() {\n"
    "  gl_Position = position*scale;\n"
    // This line exists to trick the compiler into putting a value in the compiler
    // constant bank, so we can exercise binding that bank
    "  if (scale.z != 1.0 + 1.0/65536.0) {\n"
    "      gl_Position = vec4(0,0,0,0);\n"
    "  }\n"
    "  ftc = tc;\n"
    "}\n";

static const char *fsstring = 
    "#version 440 core\n"
    "#define BINDGROUP(GROUP,INDEX) layout(binding = ((INDEX) | ((GROUP) << 4)))\n"
    "BINDGROUP(0, 3) uniform sampler2D tex;\n"
    "BINDGROUP(0, 2) uniform Block {\n"
    "    vec4 scale;\n"
    "};\n"
    "layout(location = 0) out vec4 color;\n"
    "in vec4 ftc;\n"
    "void main() {\n"
    "  color = texture(tex, ftc.xy);\n"
    "  if (scale.z != 1.0 + 1.0/65536.0) {\n"
    "      color = vec4(0,0,0,0);\n"
    "  }\n"
    "}\n";


// Two triangles that intersect
static RADfloat vertexData[] = {-0.5f, -0.5f, 0.5f, 
                                0.5f, -0.5f,  0.5f,
                                -0.5f, 0.5f,  0.5f,

                                0.5f, -0.5f, 0.5f,
                                -0.5f, -0.5f, 0.3f,
                                0.5f, 0.5f, 0.9f};

// Simple 0/1 texcoords in rgba8 format (used to be color data)
static RADubyte texcoordData[] = {0, 0, 0xFF, 0xFF,
                                  0xFF, 0, 0, 0xFF,
                                  0, 0xFF, 0, 0xFF,

                                  0, 0, 0xFF, 0xFF,
                                  0xFF, 0, 0, 0xFF,
                                  0, 0xFF, 0, 0xFF};

int windowWidth = 500, windowHeight = 500;
int offscreenWidth = 100, offscreenHeight = 100;

typedef enum {
    QUEUE,
    TOKEN,
    COMMAND,
} DrawMode;

DrawMode drawMode = COMMAND;
bool benchmark = false;

void InitRAD()
{
    // obfuscated string for radGetProcAddress
#ifdef _WIN32
    PFNRADGETPROCADDRESSPROC getProc = (PFNRADGETPROCADDRESSPROC)wglGetProcAddress("fs932040fd");
#elif __linux__
    PFNRADGETPROCADDRESSPROC getProc = (PFNRADGETPROCADDRESSPROC)glXGetProcAddressARB((const GLubyte *)"fs932040fd");
#endif
    radLoadProcs(getProc);

    // Create the "global" device and one queue
    device = radCreateDevice();
    queue = radCreateQueue(device, RAD_QUEUE_TYPE_GRAPHICS_AND_COMPUTE);
    if (useCopyQueue) {
        copyQueue = radCreateQueue(device, RAD_QUEUE_TYPE_GRAPHICS_AND_COMPUTE);
    }
}

#define USE_MULTISAMPLE 1

RADbuffer AllocAndFillBuffer(RADdevice device, void *data, int sizeofdata, RADbitfield access, bool useCopy)
{
    if (useCopy) {
        RADbuffer tempbo = radCreateBuffer(device);
        radBufferAccess(tempbo, RAD_COPY_READ_ACCESS_BIT);
        radBufferMapAccess(tempbo, RAD_MAP_WRITE_BIT | RAD_MAP_PERSISTENT_BIT);
        radBufferStorage(tempbo, sizeofdata);
        void *ptr = radMapBuffer(tempbo);
        memcpy(ptr, data, sizeofdata);

        RADbuffer buffer = radCreateBuffer(device);
        radBufferAccess(buffer, access | RAD_COPY_WRITE_ACCESS_BIT);
        radBufferMapAccess(buffer, 0);
        radBufferStorage(buffer, sizeofdata);

        radQueueCopyBuffer(useCopyQueue ? copyQueue : queue, tempbo, 0, buffer, 0, sizeofdata);

        radReleaseBuffer(tempbo, RAD_TAG_AUTO);
        return buffer;
    } else {
        RADbuffer buffer = radCreateBuffer(device);
        radBufferAccess(buffer, access);
        radBufferMapAccess(buffer, RAD_MAP_WRITE_BIT | RAD_MAP_PERSISTENT_BIT);
        radBufferStorage(buffer, sizeofdata);
        void *ptr = radMapBuffer(buffer);
        memcpy(ptr, data, sizeofdata);
        return buffer;
    }
}

void TestRAD()
{
    // Create programs from the device, provide them shader code and compile/link them
    RADprogram pgm = radCreateProgram(device);

    // XXX This is a hack because we don't have an IL. I'm just jamming through the strings 
    // as if they were an IL blob
    const char *source[2] = {vsstring, fsstring};
    radProgramSource(pgm, RAD_PROGRAM_FORMAT_GLSL, /*size*/2, (void *)source);

    // Create new state vectors
    RADcolorState color = radCreateColorState(device);
    RADdepthStencilState depth = radCreateDepthStencilState(device);
    RADvertexState vertex = radCreateVertexState(device);
    RADrasterState raster = radCreateRasterState(device);
    RADrtFormatState rtFormat = radCreateRtFormatState(device);


    radRtFormatColorFormat(rtFormat, 0, RAD_RGBA8);
    radRtFormatDepthFormat(rtFormat, RAD_DEPTH24_STENCIL8);

    radDepthStencilDepthTestEnable(depth, RAD_TRUE);
    radDepthStencilDepthWriteEnable(depth, RAD_TRUE);
    radDepthStencilDepthFunc(depth, RAD_DEPTH_FUNC_LESS);

    // Commented out experiments to test different state settings.    
    //radRasterDiscardEnable(raster, RAD_TRUE);
    //radRasterPolygonMode(raster, RAD_POLYGON_MODE_LINE);
    //radRasterLineWidth(raster, 10.0f);

    //radColorBlendEnable(color, /*MRT index*/0, RAD_TRUE);
    //radColorBlendFunc(color, /*MRT index*/0, RAD_BLEND_FUNC_ONE, RAD_BLEND_FUNC_ONE, RAD_BLEND_FUNC_ONE, RAD_BLEND_FUNC_ONE);
    //radColorMask(color, /*MRT index*/0, RAD_TRUE, RAD_TRUE, RAD_TRUE, RAD_TRUE);
    //radColorLogicOpEnable(color, RAD_TRUE);
    //radColorLogicOp(color, RAD_LOGIC_OP_XOR);

    // Set the state vector to use two vertex attributes.
    //
    // Interleaved pos+color
    // position = attrib 0 = 3*float at relativeoffset 0
    // texcoord = attrib 1 = rgba8 at relativeoffset 0
    radVertexAttribFormat(vertex, 0, 3, sizeof(RADfloat), RAD_ATTRIB_TYPE_FLOAT, 0);
    radVertexAttribFormat(vertex, 1, 4, sizeof(RADubyte), RAD_ATTRIB_TYPE_UNORM, 0);
    radVertexAttribBinding(vertex, 0, 0);
    radVertexAttribBinding(vertex, 1, 1);
    radVertexBindingGroup(vertex, 0, /*group*/0, /*index*/0);
    radVertexBindingGroup(vertex, 1, /*group*/0, /*index*/1);
    radVertexAttribEnable(vertex, 0, RAD_TRUE);
    radVertexAttribEnable(vertex, 1, RAD_TRUE);
    radVertexBindingStride(vertex, 0, 12);
    radVertexBindingStride(vertex, 1, 4);

    // Create a pipeline.
    RADpipeline pipeline = radCreatePipeline(device, RAD_PIPELINE_TYPE_GRAPHICS);

    // Attach program and state objects to the pipeline.
    radPipelineProgramStages(pipeline, RAD_VERTEX_SHADER_BIT | RAD_FRAGMENT_SHADER_BIT, pgm);
    radPipelineColorState(pipeline, color);
    radPipelineDepthStencilState(pipeline, depth);
    radPipelineVertexState(pipeline, vertex);
    radPipelineRasterState(pipeline, raster);
    radPipelineRtFormatState(pipeline, rtFormat);
    radPipelinePrimitiveType(pipeline, RAD_TRIANGLES);

    // Create a vertex buffer and fill it with data
    RADbuffer vbo = radCreateBuffer(device);
    radBufferAccess(vbo, RAD_VERTEX_ACCESS_BIT);
    radBufferMapAccess(vbo, RAD_MAP_WRITE_BIT | RAD_MAP_PERSISTENT_BIT);
    radBufferStorage(vbo, sizeof(vertexData)+sizeof(texcoordData));
    // create persistent mapping
    void *ptr = radMapBuffer(vbo);
    // fill ptr with vertex data followed by color data
    memcpy(ptr, vertexData, sizeof(vertexData));
    memcpy((char *)ptr + sizeof(vertexData), texcoordData, sizeof(texcoordData));

    // Get a handle to be used for setting the buffer as a vertex buffer
    RADvertexHandle vboHandle = radGetVertexHandle(vbo);

    // Create an index buffer and fill it with data
    unsigned short indexData[6] = {0, 1, 2, 3, 4, 5};
    RADbuffer ibo = AllocAndFillBuffer(device, indexData, sizeof(indexData), RAD_INDEX_ACCESS_BIT, true);

    // Get a handle to be used for setting the buffer as an index buffer
    RADvertexHandle iboHandle = radGetVertexHandle(ibo);

    float scale = 1.5f;
    if (benchmark) {
        scale = 0.2f;
    }
    float uboData[4] = {scale, scale, 1.0f + 1.0f/65536.0, 1.0f};
    RADbuffer ubo = AllocAndFillBuffer(device, uboData, sizeof(uboData), RAD_UNIFORM_ACCESS_BIT, false);

    // Get a handle to be used for setting the buffer as a uniform buffer
    RADuniformHandle uboHandle = radGetUniformHandle(ubo);

#if USE_MULTISAMPLE
    RADtexture rtTex = radCreateTexture(device);
    radTextureAccess(rtTex, RAD_RENDER_TARGET_ACCESS_BIT);
    radTextureStorage(rtTex, RAD_TEXTURE_2D_MULTISAMPLE, 1, RAD_RGBA8, offscreenWidth, offscreenHeight, 1, 4);

    RADtexture depthTex = radCreateTexture(device);
    radTextureAccess(depthTex, RAD_RENDER_TARGET_ACCESS_BIT);
    radTextureStorage(depthTex, RAD_TEXTURE_2D_MULTISAMPLE, 1, RAD_DEPTH24_STENCIL8, offscreenWidth, offscreenHeight, 1, 4);

    RADtexture tex1x = radCreateTexture(device);
    radTextureAccess(tex1x, RAD_RENDER_TARGET_ACCESS_BIT);
    radTextureStorage(tex1x, RAD_TEXTURE_2D, 1, RAD_RGBA8, offscreenWidth, offscreenHeight, 1, 0);

    // Create and bind a rendertarget handle
    RADrenderTargetHandle rtHandle = radGetTextureRenderTargetHandle(rtTex, RAD_TEXTURE_2D, RAD_RGBA8, 0, 0, 1);
    RADrenderTargetHandle depthHandle = radGetTextureRenderTargetHandle(depthTex, RAD_TEXTURE_2D, RAD_DEPTH24_STENCIL8, 0, 0, 1);

    radRasterSamples(raster, 4);
    radRtFormatColorSamples(rtFormat, 4);
    radRtFormatDepthStencilSamples(rtFormat, 4);

    RADpass pass = radCreatePass(device);
    radPassRenderTargets(pass, 1, &rtHandle, depthHandle, 0);
    radPassResolve(pass, RAD_RT_ATTACHMENT_COLOR0, tex1x);
#else
    RADtexture rtTex = radCreateTexture(device);
    radTextureAccess(rtTex, RAD_RENDER_TARGET_ACCESS_BIT);
    radTextureStorage(rtTex, RAD_TEXTURE_2D, 1, RAD_RGBA8, offscreenWidth, offscreenHeight, 1, 0);

    RADtexture depthTex = radCreateTexture(device);
    radTextureAccess(depthTex, RAD_RENDER_TARGET_ACCESS_BIT);
    radTextureStorage(depthTex, RAD_TEXTURE_2D, 1, RAD_DEPTH24_STENCIL8, offscreenWidth, offscreenHeight, 1, 0);

    // Create and bind a rendertarget handle
    RADrenderTargetHandle rtHandle = radGetTextureRenderTargetHandle(rtTex, RAD_TEXTURE_2D, RAD_RGBA8, 0, 0, 1);
    RADrenderTargetHandle depthHandle = radGetTextureRenderTargetHandle(depthTex, RAD_TEXTURE_2D, RAD_DEPTH24_STENCIL8, 0, 0, 1);

    RADpass pass = radCreatePass(device);
    radPassRenderTargets(pass, 1, &rtHandle, depthHandle, 0);
#endif

    radCompilePass(pass);

    radCompilePipeline(pipeline);
    RADpipelineHandle pipelineHandle = radGetPipelineHandle(pipeline);

    RADsampler sampler = radCreateSampler(device);
    // Commented out experiments to test different state settings.    
    //radSamplerMinMagFilter(sampler, RAD_MIN_FILTER_NEAREST, RAD_MAG_FILTER_NEAREST);

    const int texWidth = 4, texHeight = 4;
    RADtexture texture = radCreateTexture(device);
    radTextureAccess(texture, RAD_TEXTURE_ACCESS_BIT);
    radTextureStorage(texture, RAD_TEXTURE_2D, 1, RAD_RGBA8, texWidth, texHeight, 1, 0);
    RADtextureHandle texHandle = radGetTextureSamplerHandle(texture, sampler, RAD_TEXTURE_2D, RAD_RGBA8, 
                                                            /*minLevel*/0, /*numLevels*/1, /*minLayer*/0, /*numLayers*/1);

    RADbuffer pbo = radCreateBuffer(device);
    radBufferAccess(pbo, RAD_COPY_READ_ACCESS_BIT);
    radBufferMapAccess(pbo, RAD_MAP_WRITE_BIT | RAD_MAP_PERSISTENT_BIT);
    radBufferStorage(pbo, texWidth*texHeight*4);

    unsigned char *texdata = (unsigned char *)radMapBuffer(pbo);
    // fill with texture data
    for (int j = 0; j < texWidth; ++j) {
        for (int i = 0; i < texHeight; ++i) {
            texdata[4*(j*texWidth+i)+0] = 0xFF*((i+j)&1);
            texdata[4*(j*texWidth+i)+1] = 0x00*((i+j)&1);
            texdata[4*(j*texWidth+i)+2] = ~(0xFF*((i+j)&1));
            texdata[4*(j*texWidth+i)+3] = 0xFF;
        }
    }

    // XXX missing pixelpack object
    // Download the texture data
    radQueueCopyBufferToImage(queue, pbo, 0, texture, 0, 0, 0, 0, texWidth, texHeight, 1);

    radQueueBeginPass(queue, pass);

    // Some scissored clears
    {
        radQueueScissor(queue, 0, 0, offscreenWidth, offscreenHeight);
        float clearColor[4] = {0, 0, 0, 1};
        radQueueClearColor(queue, 0, clearColor);
        radQueueClearDepth(queue, 1.0f);
    }
    {
        radQueueScissor(queue, 0, 0, offscreenWidth/2, offscreenHeight/2);
        float clearColor[4] = {0, 1, 0, 1};
        radQueueClearColor(queue, 0, clearColor);
    }
    {
        radQueueScissor(queue, offscreenWidth/2, offscreenHeight/2, offscreenWidth/2, offscreenHeight/2);
        float clearColor[4] = {0, 0, 1, 1};
        radQueueClearColor(queue, 0, clearColor);
    }
    radQueueScissor(queue, 0, 0, offscreenWidth, offscreenHeight);
    radQueueViewport(queue, 0, 0, offscreenWidth, offscreenHeight);

    RADbindGroupElement b[4] = {{vboHandle, 0, ~0}, {vboHandle, sizeof(vertexData), ~0}, {uboHandle, 0, 4*sizeof(float)}, {texHandle, 0, ~0}};
    RADbuffer bindGroup = AllocAndFillBuffer(device, b, sizeof(b), RAD_BINDGROUP_ACCESS_BIT, false);

    RADbindGroupHandle bindGroupHandle = radGetBindGroupHandle(bindGroup);

    if (useCopyQueue) {
        // Sync from copy queue to graphics queue. Note that we currently don't sync in the 
        // opposite direction at the end of the frame, because radQueuePresent effectively
        // does a Finish so it isn't needed.
        RADsync sync = radCreateSync(device);
        radQueueFenceSync(copyQueue, sync, RAD_SYNC_ALL_GPU_COMMANDS_COMPLETE, 0);
        radQueueWaitSync(queue, sync);
        radReleaseSync(sync);
    }

    clock_t startTime = clock();
    unsigned int numIterations = benchmark ? 1000000 : 1;

    switch (drawMode) {
    case QUEUE:
        for (unsigned int i = 0; i < numIterations; ++i) {
            // Bind the pipeline, bind vertex arrays and textures, and draw
            radQueueBindPipeline(queue, RAD_PIPELINE_TYPE_GRAPHICS, pipelineHandle);
            radQueueBindGroup(queue, RAD_VERTEX_SHADER_BIT | RAD_FRAGMENT_SHADER_BIT, 0, 4, bindGroupHandle, 0);
            //radQueueDrawArrays(queue, RAD_TRIANGLES, 0, 6);
            radQueueDrawElements(queue, RAD_TRIANGLES, RAD_INDEX_UNSIGNED_SHORT, 6, iboHandle, 0);
        }
        break;
    case TOKEN:
        {
            typedef struct Draw {
                RADuint pipelineHeader;
                RADtokenBindGraphicsPipeline pipeline;
                RADuint bindGroupHeader;
                RADtokenBindGroup bindGroup;
                RADuint drawElementsHeader;
                RADtokenDrawElements drawElements;
            } Draw;
            Draw d;
            memset(&d, 0, sizeof(d));
            d.pipelineHeader = radGetTokenHeader(device, RAD_TOKEN_BIND_GRAPHICS_PIPELINE);
            d.pipeline.pipelineHandle = pipelineHandle;
            d.bindGroupHeader = radGetTokenHeader(device, RAD_TOKEN_BIND_GROUP);
            d.bindGroup.stages = RAD_VERTEX_SHADER_BIT | RAD_FRAGMENT_SHADER_BIT;
            d.bindGroup.group = 0;
            d.bindGroup.groupHandle = bindGroupHandle;
            d.bindGroup.offset = 0;
            d.bindGroup.count = 4;
            d.drawElementsHeader = radGetTokenHeader(device, RAD_TOKEN_DRAW_ELEMENTS);
            d.drawElements.indexHandle = iboHandle;
            d.drawElements.mode = RAD_TRIANGLES;
            d.drawElements.type = RAD_INDEX_UNSIGNED_SHORT;
            d.drawElements.count = 6;
            for (unsigned int i = 0; i < numIterations; ++i) {
                radQueueSubmitDynamic(queue, &d, sizeof(d));
            }
        }
        break;
    case COMMAND:
        {
            RADcommandBuffer cmd = radCreateCommandBuffer(device, RAD_QUEUE_TYPE_GRAPHICS_AND_COMPUTE);
            radCmdBindPipeline(cmd, RAD_PIPELINE_TYPE_GRAPHICS, pipelineHandle);
            radCmdBindGroup(cmd, RAD_VERTEX_SHADER_BIT | RAD_FRAGMENT_SHADER_BIT, 0, 4, bindGroupHandle, 0);
            radCmdDrawElements(cmd, RAD_TRIANGLES, RAD_INDEX_UNSIGNED_SHORT, 6, iboHandle, 0);
            radCompileCommandBuffer(cmd);
            RADcommandHandle cmdHandle = radGetCommandHandle(cmd);
            for (unsigned int i = 0; i < numIterations; ++i) {
                radQueueSubmitCommands(queue, 1, &cmdHandle);
            }
            radReleaseCommandBuffer(cmd);
        }
        break;
    }

    if (benchmark) {
        clock_t currentTime = clock();
        printf("%f\n", 1.0f*numIterations*CLOCKS_PER_SEC/(currentTime - startTime));
    }
    
    radQueueEndPass(queue, pass);

    // Kickoff submitted command buffers for the queue
    //radFlushQueue(queue);

#if USE_MULTISAMPLE
    radQueuePresent(queue, tex1x);
#else
    radQueuePresent(queue, rtTex);
#endif

    radReleaseProgram(pgm);
    radReleaseColorState(color);
    radReleaseDepthStencilState(depth);
    radReleaseVertexState(vertex);
    radReleaseRasterState(raster);
    radReleaseRtFormatState(rtFormat);
    radReleaseBuffer(vbo, RAD_TAG_AUTO);
    radReleaseBuffer(ibo, RAD_TAG_AUTO);
    radReleaseBuffer(pbo, RAD_TAG_AUTO);
    radReleaseBuffer(ubo, RAD_TAG_AUTO);
    radReleaseBuffer(bindGroup, RAD_TAG_AUTO);
    radReleaseTexture(texture, RAD_TAG_AUTO);
    radReleaseTexture(rtTex, RAD_TAG_AUTO);
    radReleaseTexture(depthTex, RAD_TAG_AUTO);
    radReleaseSampler(sampler);
    radReleasePipeline(pipeline);
    radReleasePass(pass);
#if USE_MULTISAMPLE
    radReleaseTexture(tex1x, RAD_TAG_AUTO);
#endif
}

