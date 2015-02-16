// P2: Vertex shader + fragment shader
// P3: Vertex shader + geometry shader + fragment shader

//SS("version.ss") - this doesn't work syet

// Setup
SHADER("pointpassthrough.vert", kVertexShader)
SHADER("emitquad.geom", kGeometryShader)

#define PP(NAME, SHADERPATH) P3(NAME, "pointpassthrough.vert", "emitquad.geom", SHADERPATH)

// Test shader
SHADER("test.frag", kFragmentShader)
PP("test", "test.frag")
MAT_BEGIN(ZMTest, "test")
MAT_END()

// Ground shader
SHADER("ground/ground.vsh", kVertexShader)
SHADER("ground/ground.fsh", kFragmentShader)
P2("ground", "ground/ground.vsh", "ground/ground.fsh")
MAT_BEGIN(ZMGround, "ground")
	UNI_FLOAT(dist)
	UNI_VEC3(skycol)
MAT_END()

// Ground shader with transform
SHADER("ground/ground_trans.vsh", kVertexShader)
SHADER("ground/ground_trans.fsh", kFragmentShader)
P2("groundtrans", "ground/ground_trans.vsh", "ground/ground_trans.fsh")
MAT_BEGIN(ZMGroundTrans, "groundtrans")
	UNI_FLOAT(dist)
	UNI_VEC3(skycol)
MAT_END()

// Fire particle system
SHADER("particles.gsh", kGeometryShader)
SHADER("particles.fsh", kFragmentShader)
P3("particles", "pointpassthrough.vert", "particles.gsh", "particles.fsh")
MAT_BEGIN(ZMParticles, "particles")
	UNI_FLOAT(time)
MAT_END()


// Simplicity shader
SHADER("simplicity.frag", kFragmentShader)
PP("simplicity", "simplicity.frag")
MAT_BEGIN(ZMSimplicity, "simplicity")
	UNI_FLOAT(iGlobalTime)
MAT_END()

// Simplicity shader
SHADER("knob.frag", kFragmentShader)
PP("knob", "knob.frag")
MAT_BEGIN(ZMKnob, "knob")
	UNI_FLOAT(iGlobalTime)
MAT_END()


// Base/example material
SHADER("base.vert", kVertexShader)
SHADER("base.frag", kFragmentShader)
P2("base", "base.vert", "base.frag")
MAT_BEGIN(ZMBase, "base")
MAT_END()

SHADER("envdraw.vert", kVertexShader)
SHADER("envdraw.frag", kFragmentShader)
P2("envdraw", "envdraw.vert", "envdraw.frag")
MAT_BEGIN(ZMEnvDraw, "envdraw")
	UNI_INT(tex)
MAT_END()

SHADER("env.frag", kFragmentShader)
P2("envgen", "base.vert", "env.frag")
MAT_BEGIN(ZMEnvGen, "envgen")
MAT_END()

SHADER("cube_blur.frag", kFragmentShader)
P2("cube_blur", "base.vert", "cube_blur.frag")
MAT_BEGIN(ZMPCubeBlur, "cube_blur")
	UNI_INT(tex)
	UNI_INT(numSamples)
	UNI_FLOAT(exp)
MAT_END()

// Particle draw material
SHADER("particle_draw.vert", kVertexShader)
SHADER("particle_draw.frag", kFragmentShader)
P2("particle_draw", "particle_draw.vert", "particle_draw.frag")
MAT_BEGIN(ZMParticleDraw, "particle_draw")
	UNI_INT(tex)
	UNI_INT(texOld)
	UNI_INT(envTexDiffuse)
	UNI_INT(envTexSpecular)
	UNI_FLOAT(timeFrac)
	UNI_VEC2(texSize)
MAT_END()

// Particle init material
SHADER("particle_init.frag", kFragmentShader)
PP("particle_init", "particle_init.frag")
MAT_BEGIN(ZMParticleInit, "particle_init")
	UNI_MAT4(sphereTransform)
MAT_END()

// Particle update material
SHADER("particle_update.frag", kFragmentShader)
PP("particle_update", "particle_update.frag")
MAT_BEGIN(ZMParticleUpdate, "particle_update")
	UNI_INT(oldPosTex)
	UNI_INT(curPosTex)
	UNI_FLOAT(deltaTime)
	UNI_FLOAT(prevDeltaTime)
	UNI_MAT4(sphereTransform)
MAT_END()

// Particle constrain material
SHADER("particle_constrain.frag", kFragmentShader)
PP("particle_constrain", "particle_constrain.frag")
MAT_BEGIN(ZMParticleConstrain, "particle_constrain")
	UNI_MAT4(sphereTransform)
MAT_END()

// Preetham sky shader
SHADER("sky/preetham.vsh", kVertexShader)
SHADER("sky/preetham.fsh", kFragmentShader)
P2("preetham", "sky/preetham.vsh", "sky/preetham.fsh")
MAT_BEGIN(ZMPreetham, "preetham")
MAT_END()



// Sinedistort
SHADER("sinedistort.frag", kFragmentShader)
PP("sinedistort", "sinedistort.frag")
MAT_BEGIN(ZPPSineDistort, "sinedistort")
	UNI_INT(tex)
	UNI_FLOAT(angle)
	UNI_FLOAT(frequency)
	UNI_FLOAT(amplitude)
MAT_END()

// Blend
SHADER("blend.frag", kFragmentShader)
PP("blend", "blend.frag")
MAT_BEGIN(ZPPBlend, "blend")
	UNI_INT(input0)
	UNI_INT(input1)
	UNI_INT(mode)
MAT_END()

// Bad SSAO
SHADER("bad_ssao.frag", kFragmentShader)
PP("bad_ssao", "bad_ssao.frag")
MAT_BEGIN(ZPPBadSSAO, "bad_ssao")
	UNI_INT(colorTex)
	UNI_INT(posTex)
MAT_END()

// Vignette
SHADER("vignette.frag", kFragmentShader)
PP("vignette", "vignette.frag")
MAT_BEGIN(ZPPVignette, "vignette")
	UNI_INT(tex)
MAT_END()

// Tonemap
SHADER("tonemap.frag", kFragmentShader)
PP("tonemap", "tonemap.frag")
MAT_BEGIN(ZPPToneMap, "tonemap")
	UNI_INT(input0)
	UNI_FLOAT(exposure)
	UNI_INT(mode)
MAT_END()


// Line blur
SHADER("lineblur.frag", kFragmentShader)
PP("lineblur", "lineblur.frag")
MAT_BEGIN(ZPPLineBlur, "lineblur")
	UNI_INT(tex)
	UNI_FLOAT(fScale)
	UNI_FLOAT(fDamping)
	UNI_FLOAT(fAngle)
	UNI_FLOAT(fPass)
MAT_END()

// Glow cut
SHADER("glowcut.frag", kFragmentShader)
PP("glowcut", "glowcut.frag")
MAT_BEGIN(ZPPGlowCut, "glowcut")
	UNI_INT(input0)
	UNI_FLOAT(fThresh)
	UNI_FLOAT(fSmooth)
MAT_END()

// Glow combine
SHADER("glowcombine.frag", kFragmentShader)
PP("glowcombine", "glowcombine.frag")
MAT_BEGIN(ZPPGlowCombine, "glowcombine")
	UNI_INT(input0)
	UNI_INT(input1)
	UNI_FLOAT(fMul0)
	UNI_FLOAT(fMul1)
MAT_END()


// FXAA
SHADER("alphaluma.frag", kFragmentShader)
PP("alphaluma", "alphaluma.frag")
MAT_BEGIN(ZPPAlphaLuma, "alphaluma")
	UNI_INT(tex)
MAT_END()

SHADER("fxaa3_11.frag", kFragmentShader)
PP("fxaa3_11", "fxaa3_11.frag")
MAT_BEGIN(ZPPFXAA311, "fxaa3_11")
	UNI_INT(tex)
MAT_END()

// Passthrough
SHADER("passthrough.frag", kFragmentShader)
PP("passthrough", "passthrough.frag")
MAT_BEGIN(ZPPPassthrough, "passthrough")
	UNI_INT(tex)
MAT_END()

// MSSAO materials
SHADER("mssao_downsample.frag", kFragmentShader)
PP("mssao_downsample", "mssao_downsample.frag")
MAT_BEGIN(ZPPMSSAODownsample, "mssao_downsample")
	UNI_INT(hiResPosTex)
	UNI_INT(hiResNormTex)
MAT_END()

SHADER("mssao_blur.frag", kFragmentShader)
PP("mssao_blur", "mssao_blur.frag")
MAT_BEGIN(ZPPMSSAOBlur, "mssao_blur")
	UNI_INT(posTex)
	UNI_INT(normTex)
	UNI_INT(aoTex)
MAT_END()

SHADER("mssao_first_ao.frag", kFragmentShader)
PP("mssao_first_ao", "mssao_first_ao.frag")
MAT_BEGIN(ZPPMSSAOFirstAO, "mssao_first_ao")
	UNI_INT(posTex)
	UNI_INT(normTex)

	UNI_FLOAT(dMax)
	UNI_FLOAT(rMax)
	UNI_FLOAT(r)
MAT_END()

SHADER("mssao_ao.frag", kFragmentShader)
PP("mssao_ao", "mssao_ao.frag")
MAT_BEGIN(ZPPMSSAOAO, "mssao_ao")
	UNI_INT(posTex)
	UNI_INT(normTex)

	UNI_INT(loResPosTex)
	UNI_INT(loResNormTex)
	UNI_INT(loResAOTex)

	UNI_INT(final)

	UNI_FLOAT(dMax)
	UNI_FLOAT(rMax)
	UNI_FLOAT(r)
MAT_END()
