#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OvertoneFilterEditor::OvertoneFilterEditor(OvertoneFilterAudioProcessor& p,
                                           ParameterHelper& ph,
                                           MidiKeyboardState& ks,
                                           LevelMeterAudioSource& inputLevel,
                                           LevelMeterAudioSource& wetMixLevel,
                                           LevelMeterAudioSource& outputLevel,
                                           SpectrumSource& iss, SpectrumSource& oss)
    : AudioProcessorEditor(&p),
      processor(p),
      parameterHelper(ph),
      keyboardState(ks),
      keyboard(p, ks, MidiKeyboardComponent::horizontalKeyboard, parameterHelper),
      dryMeter(inputLevel, openGLContext),
      wetMeter(wetMixLevel, openGLContext),
      outputMeter(outputLevel, openGLContext),
      spectrumDisplay(p, openGLContext, iss, oss, ph),
      mixSlider(openGLContext, ph)
{
    openGLContext.setOpenGLVersionRequired(OpenGLContext::OpenGLVersion::openGL3_2);

    setOpaque(true);
    openGLContext.setRenderer(this);
    openGLContext.attachTo(*this);
    openGLContext.setContinuousRepainting(true);

    // --------
    {
        auto& lookAndFeel = getLookAndFeel();
        lookAndFeel.setColour(Label::textColourId, Colours::black);
        lookAndFeel.setColour(Slider::thumbColourId, Colours::white);
        lookAndFeel.setColour(Slider::textBoxBackgroundColourId, Colours::black);
    }
    // --------
    const auto textEntryBoxWidth = 64;
    {
        standardSlider.setTextBoxStyle(Slider::TextBoxBelow, false, textEntryBoxWidth, 16);
        standardSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);

        addAndMakeVisible(standardSlider);
        standardAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState,
                                                      parameterHelper.pidPitchStandard, standardSlider));

        standardLabel.setText("Pitch Standard", dontSendNotification);
        standardLabel.setJustificationType(Justification::centred);
        addAndMakeVisible(standardLabel);
    }
    {
        qSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        qSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        qSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(qSlider);
        qAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.pidResonance, qSlider));

        qLabel.setText("Resonance", dontSendNotification);
        qLabel.setJustificationType(Justification::centred);
        addAndMakeVisible(qLabel);
    }
    {
        mixSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        mixSlider.setSliderStyle(Slider::LinearHorizontal);
        mixSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(mixSlider);
        mixAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.pidMix,
                                                 mixSlider));

        mixLabel.setText("Mix", dontSendNotification);
        addAndMakeVisible(mixLabel);
    }
    {
        mixAttackSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        mixAttackSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        mixAttackSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(mixAttackSlider);
        mixAttackAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState,
                                                       parameterHelper.pidMixAttack,
                                                       mixAttackSlider));

        mixAttackLabel.setText("Attack", dontSendNotification);
        mixAttackLabel.setJustificationType(Justification::centred);
        addAndMakeVisible(mixAttackLabel);
    }
    {
        mixReleaseSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        mixReleaseSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        mixReleaseSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(mixReleaseSlider);
        mixReleaseAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState,
                                                        parameterHelper.pidMixRelease,
                                                        mixReleaseSlider));

        mixReleaseLabel.setText("Release", dontSendNotification);
        mixReleaseLabel.setJustificationType(Justification::centred);
        addAndMakeVisible(mixReleaseLabel);
    }
    {
        internalMix.setRange(0.0, 1.0);
        internalMix.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        internalMix.setSliderStyle(Slider::LinearHorizontal);
        internalMix.setPopupDisplayEnabled(false, false, this);
        addAndMakeVisible(internalMix);
        startTimer(50);
    }
    {
        dryGainSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        dryGainSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        dryGainSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(dryGainSlider);
        inputGainAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.pidInputGain,
                                                       dryGainSlider));
    }
    {
        wetGainSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        wetGainSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        wetGainSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(wetGainSlider);
        wetGainAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.pidWetGain,
                                                     wetGainSlider));
    }
    {
        outputGainSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        outputGainSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        outputGainSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(outputGainSlider);
        outputGainAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.pidOutputGain,
                                                        outputGainSlider));
    }
    {
        nameLabel.setFont(30);
        nameLabel.setText("Overtone Filter - Luke M. Craig - " __DATE__ + String(" ") + __TIME__, dontSendNotification);
        nameLabel.setJustificationType(Justification::centred);
        nameLabel.setColour(Label::textColourId, Colours::white);
        addAndMakeVisible(nameLabel);
    }
    {
        addAndMakeVisible(dryMeter);
        addAndMakeVisible(wetMeter);
        addAndMakeVisible(outputMeter);

        dryMeterLabel.setText("Dry", dontSendNotification);
        wetMeterLabel.setText("Wet", dontSendNotification);
        outputMeterLabel.setText("Output", dontSendNotification);

        dryMeterLabel.setJustificationType(Justification::centred);
        wetMeterLabel.setJustificationType(Justification::centred);
        outputMeterLabel.setJustificationType(Justification::centred);

        addAndMakeVisible(dryMeterLabel);
        addAndMakeVisible(wetMeterLabel);
        addAndMakeVisible(outputMeterLabel);
    }
    {
        makeLabelUpperCase(nameLabel);

        makeLabelUpperCase(standardLabel);
        makeLabelUpperCase(qLabel);
        makeLabelUpperCase(mixLabel);
        makeLabelUpperCase(inputGainLabel);
        makeLabelUpperCase(wetGainLabel);
        makeLabelUpperCase(outputGainLabel);

        makeLabelUpperCase(dryMeterLabel);
        makeLabelUpperCase(wetMeterLabel);
        makeLabelUpperCase(outputMeterLabel);

        makeLabelUpperCase(mixAttackLabel);
        makeLabelUpperCase(mixReleaseLabel);
    }
    addAndMakeVisible(spectrumDisplay);
    addAndMakeVisible(keyboard);
    setResizable(true, true);

    setSize(1240, 680);
}

OvertoneFilterEditor::~OvertoneFilterEditor()
{
    shutdownOpenGL();
    jassert(! openGLContext.isAttached());
}

void OvertoneFilterEditor::makeLabelUpperCase(Label& label)
{
    label.setText(label.getText().toUpperCase(), dontSendNotification);
}

//==============================================================================
void OvertoneFilterEditor::paint(Graphics& g)
{
    //g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colour(0xFF000000));
    g.fillRect(keyboard.getBounds().expanded(10, 10).withTrimmedBottom(-40));
    //g.setFont(15.0f);
}

void OvertoneFilterEditor::setLabelAreaAboveCentered(Label& label, Rectangle<int>& labelArea)
{
    label.setBounds(
        labelArea.removeFromTop(16).withSizeKeepingCentre(
            6 + label.getFont().getStringWidth(label.getText()), 16));
}

void OvertoneFilterEditor::resized()
{
    auto area = getLocalBounds();

    area.removeFromBottom(10);
    {
        auto nameArea = area.removeFromBottom(30).withSizeKeepingCentre(
            6 + nameLabel.getFont().getStringWidth(nameLabel.getText()), 10);
        nameLabel.setPaintingIsUnclipped(true);
        nameLabel.setBounds(nameArea);
    }

    auto keyboardSpectrumArea = area.removeFromBottom(300).reduced(10, 10);
    spectrumDisplay.setBounds(keyboardSpectrumArea.removeFromTop(150));
    keyboardSpectrumArea.removeFromTop(10);
    keyboard.setBounds(keyboardSpectrumArea.removeFromTop(140));

    keyboard.setAvailableRange(0, 127);

    keyboard.setKeyWidth(keyboard.getWidth() / (75.0f));

    area.removeFromTop(20);
    area.removeFromRight(20);

    auto leftArea = area.removeFromLeft(area.proportionOfWidth(0.618));
    auto rightArea = area;

    // right area

    auto meterWidth = rightArea.proportionOfWidth(0.5);
    auto outputMeterArea = rightArea.removeFromRight(meterWidth);

    outputMeterArea.removeFromTop(outputMeterArea.getHeight() * 0.5 - meterWidth * 0.5);
    setLabelAreaAboveCentered(outputMeterLabel, outputMeterArea);
    outputGainSlider.setBounds(outputMeterArea.removeFromTop(meterWidth));
    outputMeter.setBounds(outputMeterArea.removeFromTop(32));

    auto mixSliderArea = rightArea.removeFromBottom(128);
    setLabelAreaAboveCentered(mixLabel, mixSliderArea);
    mixSlider.setBounds(mixSliderArea.removeFromTop(32));
    internalMix.setBounds(mixSliderArea.removeFromTop(32));

    auto mixAttackArea = mixSliderArea.removeFromLeft(mixSliderArea.proportionOfWidth(0.5));
    auto mixReleaseArea = mixSliderArea;

    setLabelAreaAboveCentered(mixAttackLabel, mixAttackArea);
    mixAttackSlider.setBounds(mixAttackArea);

    setLabelAreaAboveCentered(mixReleaseLabel, mixReleaseArea);
    mixReleaseSlider.setBounds(mixReleaseArea);

    rightArea.reduce(0, 20);

    auto dryMeterArea = rightArea.removeFromLeft(rightArea.proportionOfWidth(0.5));
    auto wetMeterArea = rightArea;

    setLabelAreaAboveCentered(dryMeterLabel, dryMeterArea);
    dryMeter.setBounds(dryMeterArea.removeFromBottom(32).reduced(2, 0));
    dryGainSlider.setBounds(dryMeterArea);

    setLabelAreaAboveCentered(wetMeterLabel, wetMeterArea);
    wetMeter.setBounds(wetMeterArea.removeFromBottom(32).reduced(2, 0));
    wetGainSlider.setBounds(wetMeterArea);

    // left area

    const auto nPanes = 2;
    const auto paneAreaHeight = leftArea.getHeight() / nPanes;

    auto sliderArea = leftArea.removeFromTop(paneAreaHeight).reduced(10, 10);
    sliderArea.removeFromTop(16);

    const auto nSliders = 2;
    auto sliderHeight = sliderArea.getWidth() / nSliders;

    auto standardSliderArea = sliderArea.removeFromLeft(sliderHeight);
    setLabelAreaAboveCentered(standardLabel, standardSliderArea);
    standardSlider.setBounds(standardSliderArea);

    auto qSliderArea = sliderArea.removeFromLeft(sliderHeight);
    setLabelAreaAboveCentered(qLabel, qSliderArea);
    qSlider.setBounds(qSliderArea);

    // extra boundaries for the background shader
    {
        componentMask = Image(Image::ARGB, getWidth(), getHeight(), true);

        Graphics imageG(componentMask);
        imageG.setColour(Colours::white);
        imageG.fillRect(outputMeter.getBounds());
        imageG.fillRect(outputMeterLabel.getBounds());

        imageG.fillRect(wetMeter.getBounds());
        imageG.fillRect(wetMeterLabel.getBounds());

        imageG.fillRect(dryMeter.getBounds());
        imageG.fillRect(dryMeterLabel.getBounds());

        imageG.fillRect(mixLabel.getBounds());
        imageG.fillRect(mixAttackLabel.getBounds());
        imageG.fillRect(mixReleaseLabel.getBounds());

        imageG.fillRect(standardLabel.getBounds());
        imageG.fillRect(qLabel.getBounds());
        imageG.fillRect(nameLabel.getBounds());

        imageG.fillRect(keyboard.getBounds());
        imageG.fillRect(spectrumDisplay.getBounds());
    }
}

void OvertoneFilterEditor::initialiseOpenGL()
{
    glDrawBuffers = (type_glDrawBuffers)OpenGLHelpers::getExtensionFunction("glDrawBuffers");
    glTexStorage2D = (type_glTexStorage2D)OpenGLHelpers::getExtensionFunction("glTexStorage2D");

    createShaders();

    boundariesTexture.bind();
    boundariesTexture.loadImage(componentMask);
    boundariesTexture.unbind();

    setupFBO();
}

void OvertoneFilterEditor::shutdown()
{
    shaderProgram.reset();
    bufferAProgram.reset();

    quad.reset();
    attributes.reset();
    uniforms.reset();

    attributes2.reset();
    uniforms2.reset();

    boundariesTexture.release();
}

void OvertoneFilterEditor::shutdownOpenGL()
{
    openGLContext.detach();
}

void OvertoneFilterEditor::setupFBO()
{
    // Generate and bind the frame buffer
    openGLContext.extensions.glGenFramebuffers(1, &fboHandle);
    openGLContext.extensions.glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

    //create the texture object
    glGenTextures(1, &renderTex);
    // Use texture unit 0
    openGLContext.extensions.glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, renderTex);

    //TODO how to get this outside of
    {
        const MessageManagerLock mmLock;
        auto desktopScale = Desktop::getInstance().getDisplays().getMainDisplay().scale;

        auto width = roundToInt(desktopScale * getWidth());
        auto height = roundToInt(desktopScale * getHeight());

        //TODO what to do if resolution changes?
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8, width, height, 0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        // Bind the texture to the FBO
        openGLContext.extensions.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, renderTex,
                                                        0);

        // Create the depth buffer
        openGLContext.extensions.glGenRenderbuffers(1, &depthBuf);
        openGLContext.extensions.glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
        openGLContext.extensions.glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT, width, height);
    }
    // Bind the depth buffer to the FBO
    openGLContext.extensions.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

    // Set the targets for the fragment shader output variables
    GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBufs);

    GLenum result = openGLContext.extensions.glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
    {
        DBG("Framebuffer is complete");
    }
    else
    {
        DBG("Framebuffer error: ");
    }

    // Unbind the framebuffer, and revert to default
    openGLContext.extensions.glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OvertoneFilterEditor::renderToTexture()
{
    // Viewport for the texture
    const auto desktopScale = (float)openGLContext.getRenderingScale();
    const auto width = roundToInt(desktopScale * getWidth());
    const auto height = roundToInt(desktopScale * getHeight());
    glViewport(0, 0, width, height);

    bufferAProgram->use();

    if (uniforms2->iResolution != nullptr)
    {
        uniforms2->iResolution->set(width, height);
    }

    if (uniforms2->iTime != nullptr)
    {
        const float sec = Time::getMillisecondCounterHiRes() * 0.001f;
        uniforms2->iTime->set(sec);
    }

    if (uniforms2->iFrame != nullptr)
    {
        uniforms2->iFrame->set(frameCounter);
    }

    if (uniforms2->iChannel0 != nullptr)
    {
        uniforms2->iChannel0->set(0);
    }

    if (uniforms2->iChannel1 != nullptr)
    {
        const auto boundariesTextureId = boundariesTexture.getTextureID();
        uniforms2->iChannel1->set(static_cast<GLint>(boundariesTextureId));
    }
    if (uniforms2->iChannel2 != nullptr)
    {
        uniforms2->iChannel2->set(static_cast<GLint>(renderTex));
    }

    // render texture scene
    quad->draw(openGLContext, *attributes2);
}

void OvertoneFilterEditor::renderScene()
{
    //render scene
    const auto desktopScale = static_cast<float>(openGLContext.getRenderingScale());
    const auto width = roundToInt(desktopScale * getWidth());
    const auto height = roundToInt(desktopScale * getHeight());
    glViewport(0, 0, width, height);

    shaderProgram->use();

    if (uniforms->iResolution != nullptr)
    {
        uniforms->iResolution->set(width, height);
    }

    if (uniforms->iChannel0 != nullptr)
    {
        // Use the texture that is associated with the FBO
        uniforms->iChannel0->set(0);
    }

    quad->draw(openGLContext, *attributes);
}

void OvertoneFilterEditor::render()
{
    jassert(OpenGLHelpers::isContextActive());
    OpenGLHelpers::clear(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    openGLContext.extensions.glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, renderTex);

    {
        openGLContext.extensions.glActiveTexture(GL_TEXTURE0 + 2);
        boundariesTexture.bind();
    }

    // Bind to texture's FBO
    openGLContext.extensions.glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

    renderToTexture();

    glFlush();

    // Unbind texture's FBO
    openGLContext.extensions.glBindFramebuffer(GL_FRAMEBUFFER, 0);

    renderScene();

    // Reset the element buffers so child Components draw correctly
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    dryMeter.renderOpenGL();
    wetMeter.renderOpenGL();
    outputMeter.renderOpenGL();
    spectrumDisplay.renderOpenGL();
    mixSlider.renderOpenGL();

    // needed to use the child components as a texture. I think this is using cachedImageFrameBuffer somehow.
    openGLContext.extensions.glActiveTexture(GL_TEXTURE0 + 1);
}

void OvertoneFilterEditor::createShaders()
{
    vertexShader =
        "attribute vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(position.xy*5.0,0.0,1.0);\n"
        "}\n";

    fragmentShader =
        "uniform vec2 iResolution;\n"
        "uniform sampler2D iChannel0;\n"
        "void main()\n"
        "{\n"
        "    // Normalized pixel coordinates (from 0 to 1)\n"
        "    vec2 uv = gl_FragCoord.xy/iResolution.xy;\n"
        "    vec3 bg = vec3(.518, .698, .353);\n"
        "    vec3 fg = vec3( .208, 0.196, 0.475);\n"
        "    vec3 col = mix(fg,bg,1.-vec3(texture2D(iChannel0,uv).y));  \n"

        "    vec2 uvCenter = uv * ( 1.0 - uv.xy);\n"
        "    float vignette = uvCenter.x * uvCenter.y * 15.0;\n"
        "    vignette = pow(vignette, 0.1);\n"
        "    gl_FragColor = vec4(col*vignette,1.0);\n"
        "}\n";

    textureShader =
        "uniform float iTime;\n"
        "uniform int iFrame;\n"
        "uniform vec2 iResolution;\n"

        "uniform sampler2D iChannel0;\n"
        "uniform sampler2D iChannel1;\n"
        "uniform sampler2D iChannel2;\n"
        "#define d_a 1.0\n"
        "#define d_b 0.4\n"
        "#define f 0.0367\n"
        "//#define k 0.06\n"
        "#define k k3(uv)\n"
        "float k3(vec2 uv){\n"
        "        return (1.0 - texture2D(iChannel1,uv).r )*.06;\n"
        "}\n"
        "\n"
        "vec4 laplace(vec2 uv, sampler2D iChannel0, vec2 iResolution){\n"
        "  vec2 p = 1. / iResolution.xy;\n"
        "  return texture2D(iChannel0,uv) * -1.0\n"
        "  + texture2D(iChannel0,uv+vec2(-1,0)*p) * 0.2\n"
        "  + texture2D(iChannel0,uv+vec2(1,0)*p) * 0.2\n"
        "  + texture2D(iChannel0,uv+vec2(0,1)*p) * 0.2\n"
        "  + texture2D(iChannel0,uv+vec2(0,-1)*p) * 0.2\n"
        "  + texture2D(iChannel0,uv+vec2(-1,-1)*p) * 0.05\n"
        "  + texture2D(iChannel0,uv+vec2(1,-1)*p) * 0.05\n"
        "  + texture2D(iChannel0,uv+vec2(1,1)*p) * 0.05\n"
        "  + texture2D(iChannel0,uv+vec2(-1,1)*p) * 0.05; \n"
        "}\n"
        "\n"
        "float updateA(vec2 uv, sampler2D iChannel0, vec2 iResolution, float iTime){\n"
        "    float a = texture2D(iChannel0,uv).x;\n"
        "    float b = texture2D(iChannel0,uv).y;\n"
        "    return a \n"
        "        + ((d_a * laplace(uv,iChannel0,iResolution).x) \n"
        "        - (a * b * b) \n"
        "        + (f * (1.0-a)));\n"
        "}\n"
        "\n"
        "float updateB(vec2 uv, sampler2D iChannel0, vec2 iResolution, float iTime){\n"
        "    float a = texture2D(iChannel0,uv).x;\n"
        "    float b = texture2D(iChannel0,uv).y;\n"
        "    return b \n"
        "        + ((d_b * laplace(uv,iChannel0,iResolution).y) \n"
        "        + (a * b * b) \n"
        "        - ((k + f) * b));\n"
        "}"

        "float random (vec2 st) {\n"
        "    return fract(sin(dot(st.xy,\n"
        "                         vec2(12.9898,78.233)))*\n"
        "        43758.5453123);\n"
        "}"
        "void main()\n"
        "{\n"
        "    // Normalized pixel coordinates (from 0 to 1)\n"
        "    vec2 uv = gl_FragCoord.xy/iResolution.xy;\n"
        "    \n"
        "    vec2 col = vec2(1.0,0.0);   \n"
        "    // if this is the first frame\n"
        "    if(iFrame <= 1) {\n"
        "        // initialize A and B\n"
        "        col.x = 1.0; \n"
        "        col.y = random(uv);\n"
        "    } "
        "    else{\n"
        "    col.x = updateA(uv,iChannel0,iResolution, iTime);\n"
        "    col.y = updateB(uv,iChannel0,iResolution, iTime);\n"
        "    }\n"
        "\n"
        "    col.x = min(col.x,1.0-texture2D(iChannel2,uv).a);\n"
        "    col.y = clamp(col.y,0.0,0.9);\n"
        "    // store A and B\n"
        "    gl_FragColor = vec4(col,1.,1.);\n"
        "}";

    quad.reset(new Shape(openGLContext));

    std::unique_ptr<OpenGLShaderProgram> newShader(new OpenGLShaderProgram(openGLContext));

    if (newShader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(vertexShader))
        && newShader->addFragmentShader("#version 410\n"
            "out " JUCE_MEDIUMP " vec4 fragColor;\n"
            + String(fragmentShader).replace("varying", "in")
                                    .replace("texture2D", "texture")
                                    .replace("gl_FragColor", "fragColor"))
        && newShader->link())
    {
        attributes.reset();
        uniforms.reset();

        shaderProgram.reset(newShader.release());
        shaderProgram->use();

        attributes.reset(new Attributes(openGLContext, *shaderProgram));
        uniforms.reset(new Uniforms(openGLContext, *shaderProgram));
    }
    else
    {
        DBG(newShader->getLastError());
    }

    std::unique_ptr<OpenGLShaderProgram> newShader2(new OpenGLShaderProgram(openGLContext));

    if (newShader2->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(vertexShader))
        && newShader2->addFragmentShader("#version 410\n"
            "out " JUCE_MEDIUMP " vec4 fragColor;\n"
            + String(textureShader).replace("varying", "in")
                                   .replace("texture2D", "texture")
                                   .replace("gl_FragColor", "fragColor"))
        && newShader2->link())
    {
        attributes2.reset();
        uniforms2.reset();

        bufferAProgram.reset(newShader2.release());
        bufferAProgram->use();

        attributes2.reset(new Attributes(openGLContext, *bufferAProgram));
        uniforms2.reset(new Uniforms(openGLContext, *bufferAProgram));
    }
    else
    {
        DBG(newShader2->getLastError());
    }
}

void OvertoneFilterEditor::newOpenGLContextCreated()
{
    initialiseOpenGL();
    dryMeter.initialiseOpenGL();
    wetMeter.initialiseOpenGL();
    outputMeter.initialiseOpenGL();
    spectrumDisplay.initialiseOpenGL();
    mixSlider.initialiseOpenGL();
}

void OvertoneFilterEditor::renderOpenGL()
{
    render();
    ++frameCounter;
}

void OvertoneFilterEditor::openGLContextClosing()
{
    dryMeter.shutdown();
    wetMeter.shutdown();
    outputMeter.shutdown();
    spectrumDisplay.shutdown();
    mixSlider.shutdown();
    shutdown();
}

//==============================================================================
void OvertoneFilterEditor::timerCallback()
{
    internalMix.setValue(parameterHelper.getCurrentMix(0));
}

//==============================================================================

OvertoneFilterEditor::Uniforms::Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
{
    iResolution.reset(createUniform(openGLContext, shaderProgram, "iResolution"));
    iTime.reset(createUniform(openGLContext, shaderProgram, "iTime"));
    iFrame.reset(createUniform(openGLContext, shaderProgram, "iFrame"));
    iChannel0.reset(createUniform(openGLContext, shaderProgram, "iChannel0"));
    iChannel1.reset(createUniform(openGLContext, shaderProgram, "iChannel1"));
    iChannel2.reset(createUniform(openGLContext, shaderProgram, "iChannel2"));
}

OpenGLShaderProgram::Uniform* OvertoneFilterEditor::Uniforms::createUniform(OpenGLContext& openGLContext,
                                                                            OpenGLShaderProgram&
                                                                            shaderProgram,
                                                                            const char* uniformName)
{
    if (openGLContext.extensions.glGetUniformLocation(shaderProgram.getProgramID(), uniformName) < 0)
        return nullptr;

    return new OpenGLShaderProgram::Uniform(shaderProgram, uniformName);
}
