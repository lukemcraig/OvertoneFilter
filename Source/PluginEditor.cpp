#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OvertoneFilterEditor::OvertoneFilterEditor(OvertoneFilterAudioProcessor& p,
                                           ParameterHelper& ph,
                                           MidiKeyboardState& ks,
                                           LevelMeterAudioSource& inputLevel,
                                           LevelMeterAudioSource& wetMixLevel,
                                           LevelMeterAudioSource& outputLevel)
    : AudioProcessorEditor(&p),
      processor(p),
      parameterHelper(ph),
      keyboardState(ks),
      keyboard(p, ks, MidiKeyboardComponent::horizontalKeyboard),
      inputMeter(inputLevel, openGLContext, Colours::blueviolet),
      wetMixMeter(wetMixLevel, openGLContext, Colours::blueviolet),
      outputMeter(outputLevel, openGLContext),
      spectrumDisplay(p, openGLContext)
{
    openGLContext.setOpenGLVersionRequired(OpenGLContext::OpenGLVersion::openGL3_2);

    setOpaque(true);
    openGLContext.setRenderer(this);
    openGLContext.attachTo(*this);
    openGLContext.setContinuousRepainting(true);

    // --------
    getLookAndFeel().setColour(Label::textColourId, Colours::black);
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
        qAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.pidQ, qSlider));

        qLabel.setText("Resonance", dontSendNotification);
        qLabel.setJustificationType(Justification::centred);
        addAndMakeVisible(qLabel);
    }
    {
        mixSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        mixSlider.setSliderStyle(Slider::LinearVertical);
        mixSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(mixSlider);
        mixAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.pidMix,
                                                 mixSlider));

        mixLabel.setText("Mix", dontSendNotification);
        addAndMakeVisible(mixLabel);
    }

    {
        inputGainSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        inputGainSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        inputGainSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(inputGainSlider);
        inputGainAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.pidInputGain,
                                                       inputGainSlider));
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
        borderPath.setFill(Colours::transparentBlack);
        borderPath.setStrokeType(PathStrokeType(1));
        borderPath.setStrokeFill(Colours::white);
        addAndMakeVisible(borderPath);
    }
    {
        nameLabel.setText("Overtone Filter - Luke M. Craig - " __DATE__ + String(" ") + __TIME__, dontSendNotification);
        nameLabel.setJustificationType(Justification::centred);
        addAndMakeVisible(nameLabel);
    }
    {
        addAndMakeVisible(inputMeter);
        addAndMakeVisible(wetMixMeter);
        addAndMakeVisible(outputMeter);

        inputMeterLabel.setText("Dry", dontSendNotification);
        wetMixMeterLabel.setText("Wet", dontSendNotification);
        outputMeterLabel.setText("Output", dontSendNotification);

        inputMeterLabel.setJustificationType(Justification::centred);
        wetMixMeterLabel.setJustificationType(Justification::centred);
        outputMeterLabel.setJustificationType(Justification::centred);

        addAndMakeVisible(inputMeterLabel);
        addAndMakeVisible(wetMixMeterLabel);
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

        makeLabelUpperCase(inputMeterLabel);
        makeLabelUpperCase(wetMixMeterLabel);
        makeLabelUpperCase(outputMeterLabel);
    }
    addAndMakeVisible(spectrumDisplay);
    addAndMakeVisible(keyboard);
    setResizable(true, true);
    //setResizeLimits(400, 400, 1680, 1050);
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

    //g.setColour(Colours::transparentBlack);
    //g.fillRect(0, 0, 100, 200);
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
    //DBG(area.getWidth());
    //DBG(area.getHeight());
    // margins
    area.reduce(10, 10);

    {
        auto nameArea = area.removeFromTop(10).withSizeKeepingCentre(
            6 + nameLabel.getFont().getStringWidth(nameLabel.getText()), 10);
        nameLabel.setPaintingIsUnclipped(true);
        nameLabel.setBounds(nameArea);
    }
    {
        auto pad = 10;
        auto w = -pad + (area.getWidth() - nameLabel.getFont().getStringWidthFloat(nameLabel.getText())) / 2.0f;

        const auto topLeft = area.getTopLeft();
        const auto bottomLeft = area.getBottomLeft();
        const auto bottomRight = area.getBottomRight();
        const auto topRight = area.getTopRight();

        Path path;
        path.startNewSubPath(area.getX() + w, area.getY() - 5.0f);
        path.lineTo(topLeft.getX(), topLeft.getY() - 5.0f);
        path.lineTo(bottomLeft.getX(), bottomLeft.getY());
        path.lineTo(bottomRight.getX(), bottomRight.getY());
        path.lineTo(topRight.getX(), topRight.getY() - 5.0f);
        path.lineTo(topRight.getX() - w, topRight.getY() - 5.0f);
        auto roundPath = path.createPathWithRoundedCorners(3);
        borderPath.setPath(roundPath);
    }
    area.reduce(10, 10);

    spectrumDisplay.setBounds(area.removeFromTop(100));
    keyboard.setBounds(area.removeFromTop(200));
    //keyboard.setKeyWidth(20);
    //keyboard.setLowestVisibleKey(24);

    keyboard.setAvailableRange(0, 61);
    //auto neededWidth = keyboard.getTotalKeyboardWidth();

    keyboard.setKeyWidth(keyboard.getWidth() / (36.0f));
    //for (int i = 127; i >= 0; --i)
    //{
    //    auto ksp = keyboard.getKeyStartPosition(i);
    //    auto keyWidth = keyboard.getKeyWidth();
    //    auto lastNote = keyboard.getNoteAtPosition(Point<float>(ksp + (keyWidth * 0.5f), 0));
    //    if (lastNote != -1)
    //    {
    //        DBG(lastNote);
    //        break;
    //    }
    //}

    auto leftArea = area.removeFromLeft(area.proportionOfWidth(0.618));
    auto rightArea = area;

    // right area

    auto meterWidth = rightArea.proportionOfWidth(0.5);
    auto outputMeterArea = rightArea.removeFromRight(meterWidth);

    outputMeterArea.removeFromTop(outputMeterArea.getHeight() * 0.5 - meterWidth * 0.5);
    setLabelAreaAboveCentered(outputMeterLabel, outputMeterArea);
    outputGainSlider.setBounds(outputMeterArea.removeFromTop(meterWidth));
    outputMeter.setBounds(outputMeterArea.removeFromTop(32));

    auto mixSliderArea = rightArea.removeFromRight(32);
    mixLabel.setBounds(mixSliderArea.removeFromTop(16));
    mixSlider.setBounds(mixSliderArea);

    auto wetMixMeterArea = rightArea.removeFromTop(rightArea.proportionOfHeight(0.5));
    auto inputMeterArea = rightArea;

    setLabelAreaAboveCentered(inputMeterLabel, inputMeterArea);
    inputMeter.setBounds(inputMeterArea.removeFromBottom(32));
    inputGainSlider.setBounds(inputMeterArea);

    setLabelAreaAboveCentered(wetMixMeterLabel, wetMixMeterArea);
    wetMixMeter.setBounds(wetMixMeterArea.removeFromBottom(32));
    wetGainSlider.setBounds(wetMixMeterArea);

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

    //const auto keyboardArea = leftArea.removeFromTop(paneAreaHeight).reduced(10, 0);
    //keyboard.setBounds(keyboardArea);

    // extra boundaries for the background shader
    {
        componentMask = Image(Image::ARGB, getWidth(), getHeight(), true);

        Graphics imageG(componentMask);
        imageG.setColour(Colours::white);
        imageG.fillRect(outputMeter.getBounds());
        imageG.fillRect(outputMeterLabel.getBounds());

        imageG.fillRect(wetMixMeter.getBounds());
        imageG.fillRect(wetMixMeterLabel.getBounds());

        imageG.fillRect(inputMeter.getBounds());
        imageG.fillRect(inputMeterLabel.getBounds());

        imageG.fillRect(mixLabel.getBounds());
        imageG.fillRect(standardLabel.getBounds());
        imageG.fillRect(qLabel.getBounds());
        imageG.fillRect(nameLabel.getBounds());
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
    //1.
    // Generate and bind the frame buffer
    openGLContext.extensions.glGenFramebuffers(1, &fboHandle);
    openGLContext.extensions.glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

    //create the texture object
    glGenTextures(1, &renderTex);
    // Use texture unit 0
    openGLContext.extensions.glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTex);

    //TODO how to get this outside of
    {
        const MessageManagerLock mmLock;
        auto desktopScale = Desktop::getInstance().getDisplays().getMainDisplay().scale;

        auto width = roundToInt(desktopScale * getWidth());
        auto height = roundToInt(desktopScale * getHeight());

        //TODO what to do if resolution changes?
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8, width, height, 0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);

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
    auto desktopScale = (float)openGLContext.getRenderingScale();
    auto width = roundToInt(desktopScale * getWidth());
    auto height = roundToInt(desktopScale * getHeight());
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

    if (uniforms2->slider0 != nullptr)
    {
        //todo
        uniforms2->slider0->set(static_cast<GLfloat>(0.0367));
    }

    if (uniforms2->iChannel0 != nullptr)
    {
        uniforms2->iChannel0->set(0);
    }

    if (uniforms2->iChannel1 != nullptr)
    {
        uniforms2->iChannel1->set(1);
    }
    if (uniforms2->iChannel2 != nullptr)
    {
        uniforms2->iChannel2->set(2);
    }

    // render texture scene
    quad->draw(openGLContext, *attributes2);
}

void OvertoneFilterEditor::renderScene()
{
    //render scene
    auto desktopScale = (float)openGLContext.getRenderingScale();
    auto width = roundToInt(desktopScale * getWidth());
    auto height = roundToInt(desktopScale * getHeight());
    glViewport(0, 0, width, height);

    shaderProgram->use();

    if (uniforms->iResolution != nullptr)
    {
        uniforms->iResolution->set(width, height);
    }

    if (uniforms->iTime != nullptr)
    {
        const float sec = Time::getMillisecondCounterHiRes() * 0.001f;
        uniforms->iTime->set(sec);
    }

    if (uniforms->iFrame != nullptr)
    {
        uniforms->iFrame->set(frameCounter);
    }

    if (uniforms->slider0 != nullptr)
    {
        //todo
        uniforms->slider0->set(static_cast<GLfloat>(0.0367));
    }

    if (uniforms->iChannel0 != nullptr)
    {
        // Use the texture that is associated with the FBO
        uniforms->iChannel0->set(0);
    }
    //todo
    //if (uniforms->iSpectrum != nullptr)
    //{
    //    if (processor.nextFFTBlockReady)
    //    {
    //        processor.forwardFFT.performFrequencyOnlyForwardTransform(processor.fftData);
    //        processor.nextFFTBlockReady = false;
    //        uniforms->iSpectrum->set(processor.fftData, ShaderGui3AudioProcessor::fftSize);
    //    }
    //}

    quad->draw(openGLContext, *attributes);
}

void OvertoneFilterEditor::render()
{
    jassert(OpenGLHelpers::isContextActive());
    OpenGLHelpers::clear(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    openGLContext.extensions.glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTex);

    {
        openGLContext.extensions.glActiveTexture(GL_TEXTURE2);
        boundariesTexture.bind();
    }

    //3.
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

    inputMeter.renderOpenGL();
    wetMixMeter.renderOpenGL();
    outputMeter.renderOpenGL();
    spectrumDisplay.renderOpenGL();

    // needed to use the child components as a texture. I think this is using cachedImageFrameBuffer somehow.
    openGLContext.extensions.glActiveTexture(GL_TEXTURE1);
}

void OvertoneFilterEditor::createShaders()
{
    auto languageVersion = OpenGLShaderProgram::getLanguageVersion();
    DBG(languageVersion);
    //todo make these const
    vertexShader =
        "attribute vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(position.xy*5.0,0.0,1.0);\n"
        "}\n";

    fragmentShader =
        "uniform int iFrame;\n"
        "uniform float iTime;\n"
        "uniform vec2 iResolution;\n"
        "uniform float slider0;\n"
        "uniform sampler2D iChannel0;\n"
        //todo
        //"uniform float iSpectrum[" + String(OvertoneFilterAudioProcessor::fftSize) + "];\n"
        "void main()\n"
        "{\n"
        "    // Normalized pixel coordinates (from 0 to 1)\n"
        "    vec2 uv = gl_FragCoord.xy/iResolution.xy;\n"
        "    vec3 fg = vec3(.929, .918, .757);\n"
        "    vec3 bg = vec3( 0.1, 0.1, 0.2);\n"
        "    vec3 col = mix(bg,fg,1.-vec3(texture2D(iChannel0,uv).y));  \n"
        //"    col += vec3(sin(iSpectrum[int(uv.x * " + String(OvertoneFilterAudioProcessor::fftSize) + ")]));  \n"
        "    gl_FragColor = vec4(col,1.0);\n"
        "}\n";

    textureShader =
        "uniform float iTime;\n"
        "uniform int iFrame;\n"
        "uniform vec2 iResolution;\n"
        "uniform float slider0;\n"
        "uniform sampler2D iChannel0;\n"
        "uniform sampler2D iChannel1;\n"
        "uniform sampler2D iChannel2;\n"
        "#define d_a 1.0\n"
        "#define d_b 0.4\n"
        "//#define f 0.0367\n"
        "#define f slider0\n"
        "\n"
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
    inputMeter.initialiseOpenGL();
    wetMixMeter.initialiseOpenGL();
    outputMeter.initialiseOpenGL();
    spectrumDisplay.initialiseOpenGL();
}

void OvertoneFilterEditor::renderOpenGL()
{
    render();
    ++frameCounter;
}

void OvertoneFilterEditor::openGLContextClosing()
{
    inputMeter.shutdown();
    wetMixMeter.shutdown();
    outputMeter.shutdown();
    spectrumDisplay.shutdown();
    shutdown();
}

//==============================================================================

OvertoneFilterEditor::Attributes::Attributes(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
{
    position.reset(createAttribute(openGLContext, shaderProgram, "position"));
}

void OvertoneFilterEditor::Attributes::enable(OpenGLContext& glContext)
{
    if (position.get() != nullptr)
    {
        glContext.extensions.glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                                   nullptr);
        glContext.extensions.glEnableVertexAttribArray(position->attributeID);
    }
}

void OvertoneFilterEditor::Attributes::disable(OpenGLContext& glContext)
{
    if (position.get() != nullptr) glContext.extensions.glDisableVertexAttribArray(position->attributeID);
}

OpenGLShaderProgram::Attribute* OvertoneFilterEditor::Attributes::createAttribute(
    OpenGLContext& openGLContext, OpenGLShaderProgram& shader, const char* attributeName)
{
    if (openGLContext.extensions.glGetAttribLocation(shader.getProgramID(), attributeName) < 0)
        return nullptr;

    return new OpenGLShaderProgram::Attribute(shader, attributeName);
}

OvertoneFilterEditor::Uniforms::Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
{
    iResolution.reset(createUniform(openGLContext, shaderProgram, "iResolution"));
    iTime.reset(createUniform(openGLContext, shaderProgram, "iTime"));
    iFrame.reset(createUniform(openGLContext, shaderProgram, "iFrame"));
    slider0.reset(createUniform(openGLContext, shaderProgram, "slider0"));
    iChannel0.reset(createUniform(openGLContext, shaderProgram, "iChannel0"));
    iChannel1.reset(createUniform(openGLContext, shaderProgram, "iChannel1"));
    iChannel2.reset(createUniform(openGLContext, shaderProgram, "iChannel2"));
    iSpectrum.reset(createUniform(openGLContext, shaderProgram, "iSpectrum"));
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

OvertoneFilterEditor::Shape::Shape(OpenGLContext& glContext)
{
    //auto objFileContent = loadEntireAssetIntoString("quad.obj");
    String objFileContent{
        "v -1.000000 -1.000000 0.000000\n"
        "v 1.000000 -1.000000 0.000000\n"
        "v -1.000000 1.000000 -0.000000\n"
        "v 1.000000 1.000000 -0.000000\n"
        "f 2 3 1\n"
        "f 2 4 3\n"
    };
    if (shapeFile.load(objFileContent).wasOk())
        for (auto* shapeVertices : shapeFile.shapes)
            vertexBuffers.add(new VertexBuffer(glContext, *shapeVertices));
}

void OvertoneFilterEditor::Shape::draw(OpenGLContext& glContext, Attributes& glAttributes)
{
    for (auto* vertexBuffer : vertexBuffers)
    {
        vertexBuffer->bind();

        glAttributes.enable(glContext);
        glDrawElements(GL_TRIANGLES, vertexBuffer->numIndices, GL_UNSIGNED_INT, nullptr);
        glAttributes.disable(glContext);
    }
}

OvertoneFilterEditor::Shape::VertexBuffer::VertexBuffer(OpenGLContext& context,
                                                        WavefrontObjFile::Shape& aShape): openGLContext(
    context)
{
    numIndices = aShape.mesh.indices.size();

    openGLContext.extensions.glGenBuffers(1, &vertexBuffer);
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    Array<Vertex> vertices;
    createVertexListFromMesh(aShape.mesh, vertices, Colours::green);

    openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER,
                                          static_cast<GLsizeiptr>(static_cast<size_t>(vertices.size()) *
                                              sizeof(Vertex)),
                                          vertices.getRawDataPointer(), GL_STATIC_DRAW);

    openGLContext.extensions.glGenBuffers(1, &indexBuffer);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    openGLContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                          static_cast<GLsizeiptr>(static_cast<size_t>(numIndices) * sizeof(
                                              juce::uint32)),
                                          aShape.mesh.indices.getRawDataPointer(), GL_STATIC_DRAW);
}

OvertoneFilterEditor::Shape::VertexBuffer::~VertexBuffer()
{
    openGLContext.extensions.glDeleteBuffers(1, &vertexBuffer);
    openGLContext.extensions.glDeleteBuffers(1, &indexBuffer);
}

void OvertoneFilterEditor::Shape::VertexBuffer::bind()
{
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
}

void OvertoneFilterEditor::Shape::createVertexListFromMesh(const WavefrontObjFile::Mesh& mesh,
                                                           Array<Vertex>& list, Colour colour)
{
    auto scale = 0.2f;
    WavefrontObjFile::TextureCoord defaultTexCoord{0.5f, 0.5f};
    WavefrontObjFile::Vertex defaultNormal{0.5f, 0.5f, 0.5f};

    for (auto i = 0; i < mesh.vertices.size(); ++i)
    {
        const auto& v = mesh.vertices.getReference(i);
        const auto& n = i < mesh.normals.size() ? mesh.normals.getReference(i) : defaultNormal;
        const auto& tc = i < mesh.textureCoords.size() ? mesh.textureCoords.getReference(i) : defaultTexCoord;

        list.add({
            {scale * v.x, scale * v.y, scale * v.z,},
            {scale * n.x, scale * n.y, scale * n.z,},
            {colour.getFloatRed(), colour.getFloatGreen(), colour.getFloatBlue(), colour.getFloatAlpha()},
            {tc.x, tc.y}
        });
    }
}
