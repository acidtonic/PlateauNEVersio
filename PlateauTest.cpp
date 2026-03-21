#define DSJ_PLATEAU_HPP

#include "Dattorro.hpp"
#include "src/Lmtr.hpp"
#include "src/bogaudio.hpp"
#include "signalsmith/envelopes.h"
#include "signalsmith/delay.h"

using namespace daisy;
using namespace daisysp;
using namespace bogaudio;

struct ExponentialRelease {
	double releaseSlew;
	double output = 1;
	
	ExponentialRelease(double releaseSamples = 1280) {
		// The exact value is `1 - exp(-1/releaseSamples)`
		// but this is a decent approximation
		releaseSlew = 1/(releaseSamples + 1);
	}
	
	double step(double input) {
		// Move towards input
		output += (input - output)*releaseSlew;
		output = std::min(output, input);
		return output;
	}
};

struct LimiterAttackHoldRelease {
	double limit = 0.85;
	double attackMs = 10;
	double holdMs = 0;
	double releaseMs = 600;
	
	signalsmith::envelopes::PeakHold<double> peakHold{0};
	signalsmith::envelopes::BoxStackFilter<double> smoother{0};
	// We don't need fractional delays, so this could be nearest-sample
	signalsmith::delay::Delay<double> delay;
	ExponentialRelease release; // see the previous example code
	
	int attackSamples = 0;
	void configure(double sampleRate) {
		attackSamples = attackMs*0.001*sampleRate;
		int holdSamples = holdMs*0.001*sampleRate;
		double releaseSamples = releaseMs*0.001*sampleRate;
		release = ExponentialRelease(releaseSamples);

		peakHold.resize(attackSamples + holdSamples);
		smoother.resize(attackSamples, 3);
		smoother.reset(1);
		
		delay.resize(attackSamples + 1);
	}
	int latencySamples() {
		return attackSamples;
	}
	
	inline double gain(const double &v) {
		double maxGain = 1;
		if (std::abs(v) > limit) {
			maxGain = limit/std::abs(v);
		}
        
		return smoother(release.step(-peakHold(-maxGain)));

	}

	double sample(const double &v) {
		return delay.write(v).read(attackSamples) * gain(v);
	}
};

DaisyVersio hw;

LimiterAttackHoldRelease softerLimiterLeft;
LimiterAttackHoldRelease softerLimiterRight;

Lmtr limiter;

const double minus18dBGain = 0.12589254;
const double minus20dBGain = 0.1;

double wet = 0.5;
double dry = 0.5;

double decay = 0.877465;
double timeScale = 0.f;

double diffusion = 1.;
double tempDiffusion = 1.;

double inputDampLow = 0.;
double inputDampHigh = 0.;

double reverbDampLow = 0.;
double reverbDampHigh = 0.;

double previousInputDampLow = 0.;
double previousInputDampHigh = 0.;

double previousReverbDampLow = 0.;
double previousReverbDampHigh = 0.;

uint32_t gainMode = 0;
unsigned int gainModeLedTimer = 32001;
const unsigned int gainModeLedOnTime = 32000;

double leftInput = 0.;
double rightInput = 0.;
double leftOutput = 0.;
double rightOutput = 0.;

double volumeChange = 0.;

unsigned int holdCount = 0;

double inputVolumeModifier = 1.;
double tempInputVolumeModifier = inputVolumeModifier;

bool buttonState = false;
bool previousButtonState = false;
unsigned int buttonHoldTimer = 0;
unsigned int buttonOffTimer = 0;
unsigned int buttonConfirmTime = 32000;

bool confirmationSequence = false;
bool confirmationSequenceOne = false;
bool confirmationSequenceTwo = false;
unsigned int confirmationSequenceCounter = 0;
unsigned int confirmationSequenceTimer = 0;
uint32_t buttonMode = 0;


unsigned int toneKnobLedTimer = 32001;
const unsigned int toneKnobLedOnTime = 32000;
bool toneKnobIsMoving = false;
double toneKnobValue = 0.;
double previousToneKnobValue = 0.;
double toneKnobZeroLockValue = 0.;
double previousToneKnobZeroLockValue = 0.;

double previousModDepthKnobValue = 0.;
bool modDepthKnobIsMoving = false;

bool lockModDepthTo3_125_ = false;

bool leds = true;

/* LibDaisy changed the API and made these private.
auto *LED0PtrRed = &hw.leds[0].r_;
auto *LED1PtrRed = &hw.leds[1].r_;
auto *LED2PtrRed = &hw.leds[2].r_;
auto *LED3PtrRed = &hw.leds[3].r_;
*/

auto &LED0PtrRed = hw.leds[0];
auto &LED1PtrRed = hw.leds[1];
auto &LED2PtrRed = hw.leds[2];
auto &LED3PtrRed = hw.leds[3];

Dattorro reverb(32000, 16, 4.0);

bool diffusionEnabled = true;

double preDelay = 0.;
double previousPreDelay = 0.;

auto *SWITCH0Ptr = &hw.sw[0];
auto *SWITCH1Ptr = &hw.sw[1];
unsigned char switchState0 = 0;
unsigned char switchState1 = 0;

auto *KNOB0Ptr = &hw.knobs[0];
auto *KNOB1Ptr = &hw.knobs[1];
auto *KNOB2Ptr = &hw.knobs[2];
auto *KNOB3Ptr = &hw.knobs[3];
auto *KNOB4Ptr = &hw.knobs[4];
auto *KNOB5Ptr = &hw.knobs[5];
auto *KNOB6Ptr = &hw.knobs[6];

double outputAmplification = 0.0;
double tempOutputAmplification = outputAmplification;

double inputAmplification = 0.0;
double tempInputAmplification = inputAmplification;

double modDepthValue = 0.;
double lockedModDepthValue = 0.;

double led1 = 0.;
double led2 = 0.;
double led3 = 0.;
double led4 = 0.;

double knobValue0 = 0.;
double knobValue1 = 0.;
double knobValue2 = 0.;
double knobValue3 = 0.;
double knobValue4 = 0.;
double knobValue5 = 0.;
double knobValue6 = 0.;

unsigned int saveTimer = 0;
bool saveTrigger = false; 
unsigned int saveTime = 32000;

bool clear = false;

bool shapeSet = true;

unsigned int lockModDepthTime = 320000;
unsigned int bufferClearTriggerWindow = 32000;

unsigned int genericLedOnTime = 32000;
unsigned int genericLedTimer = genericLedOnTime + 1;

bool freeze = false;

// Persistence
constexpr uint32_t settingsVersion = 1;

struct Settings {
    uint32_t gainMode;
    uint32_t version;
    float inputDampLow;
    float inputDampHigh;
    float reverbDampLow;
    float reverbDampHigh;
    float diffusion;
    float inputAmplification;
    float outputAmplification;

    bool operator!=(const Settings& other) const {
        return gainMode != other.gainMode
            || version != other.version
            || inputDampLow != other.inputDampLow
            || inputDampHigh != other.inputDampHigh
            || reverbDampLow != other.reverbDampLow
            || reverbDampHigh != other.reverbDampHigh
            || diffusion != other.diffusion
            || inputAmplification != other.inputAmplification
            || outputAmplification != other.outputAmplification;
    }
};

PersistentStorage<Settings> storage(hw.seed.qspi);
bool settingsDirty = false;

inline void markSettingsDirty() {
    settingsDirty = true;
    saveTrigger = false;
    saveTimer = 0;
}

inline void saveData() {

    //
    // Save settings to QSPI
    //

    Settings &localSettings = storage.GetSettings();
    localSettings.gainMode = gainMode;
    localSettings.version = settingsVersion;
    localSettings.inputDampLow = inputDampLow;
    localSettings.inputDampHigh = inputDampHigh;
    localSettings.reverbDampLow = reverbDampLow;
    localSettings.reverbDampHigh = reverbDampHigh;
    localSettings.diffusion = diffusion;
    localSettings.inputAmplification = inputAmplification;
    localSettings.outputAmplification = outputAmplification;
    storage.Save();
    settingsDirty = false;
    saveTrigger = false;
    saveTimer = 0;
}

inline void loadData() {

    //
    // Load settings from QSPI
    //

    Settings &localSettings = storage.GetSettings();
    gainMode = localSettings.gainMode;

    if(localSettings.version != settingsVersion) {
        diffusionEnabled = diffusion != 0.;
        saveData();
        return;
    }

    inputDampLow = localSettings.inputDampLow;
    inputDampHigh = localSettings.inputDampHigh;
    reverbDampLow = localSettings.reverbDampLow;
    reverbDampHigh = localSettings.reverbDampHigh;
    diffusion = localSettings.diffusion;
    inputAmplification = localSettings.inputAmplification;
    outputAmplification = localSettings.outputAmplification;
    diffusionEnabled = diffusion != 0.;
}

inline void saveCounterAudioRate() {
    if(settingsDirty) {
        if(saveTimer < saveTime) {
            ++saveTimer;
        } else {
            saveTrigger = true;
        }
    }
}

inline void saturation(double &x) {
    x = x * ( 27. + x * x ) / ( 27. + 9. * x * x );
}

// Fast hyperbolic tangent function.
inline void hardLimiter(double &x, double &y) {
    limiter.processChannel(x, y, x, y);
}

inline void softLimiter(double &x, double &y) {
   x = softerLimiterLeft.sample(x);
   y = softerLimiterRight.sample(y);
}

double hardClipGain = 0.85;
inline double hardClip(const double &x) {
    return (x > hardClipGain) ? hardClipGain : ((x < -hardClipGain) ? -hardClipGain : x);
}

inline double hardLimit50_(const double &x) {
    return (x > 0.50) ? 0.50 : ((x < -0.50) ? -0.50 : x);
}

inline double hardLimit77_8_(const double &x) {
    return (x > 0.778) ? 0.778 : ((x < -0.778) ? -0.778 : x);
}

inline double hardLimit100_(const double &x) {
    return (x > 1.) ? 1. : ((x < -1.) ? -1. : x);
}


inline double amp120_(const double &x) {
    return x * 1.2;
}

inline void foldbackDistortion(double &x, double threshold) {
    if (x>threshold || x<-threshold) {
        x = std::fabs(std::fabs(std::fmod(x - threshold, threshold*4)) - threshold*2) - threshold;
    }
}

unsigned int holdSamples = 32;
unsigned int rippedCountLeft = 0;
double leftValue = 1.;
unsigned int rippedCountRight = 0;
double rightValue = 1.;
double smoothing = 0.85;

struct GateFilter {
	double tmp = 0.;

	GateFilter() {
        inline double processLowpass(const double &x);
    }

    double processLowpass(const double &x) {
        tmp = (1 - smoothing) * x + smoothing * tmp;
        return tmp;
    }
};

GateFilter leftGateFilter;
GateFilter rightGateFilter;

inline void rippedSpeakerLeft(double& x, double threshold) {
    if(rippedCountLeft < holdSamples) {
        ++rippedCountLeft;
        leftValue = 0.;
    } else if (rippedCountLeft == holdSamples) {
        rippedCountLeft = holdSamples + 1;
        leftValue = 1.;
    }
    if (x > threshold || x < -threshold) {
        rippedCountLeft = 0;
        leftValue = 0.;
    }
    x *= leftGateFilter.processLowpass(leftValue);
}

inline void rippedSpeakerRight(double& x, double threshold) {
    if(rippedCountRight < holdSamples) {
        ++rippedCountRight;
        rightValue = 0.;
    } else if (rippedCountRight == holdSamples) {
        rippedCountRight = holdSamples + 1;
        rightValue = 1.;
    }
    if (x > threshold || x < -threshold) {
        rippedCountRight = 0;
        rightValue = 0.;
    }
    x *= rightGateFilter.processLowpass(rightValue);
}

struct KnobOnePoleFilter {
	double tmp = 0.;

	KnobOnePoleFilter() {
        inline double processLowpass(const double &x);
    }

    double processLowpass(const double &x) {
        tmp = 0.0005 * x + 0.9995 * tmp;
        return tmp;
    }
};

KnobOnePoleFilter mixKnobLPF;
KnobOnePoleFilter modDepthKnobLPF;
KnobOnePoleFilter preDelayKnobLPF;
KnobOnePoleFilter timeScaleKnobLPF;
KnobOnePoleFilter toneKnobLPF;
KnobOnePoleFilter toneKnobZeroLockLPF;
KnobOnePoleFilter decayKnobLPF;

inline void checkIfToneKnobIsMoving(double currentValue) {
    if (((currentValue - previousToneKnobValue) > 0.001) or ((currentValue - previousToneKnobValue) < -0.001)) {
        previousToneKnobValue = currentValue;
        toneKnobIsMoving = true;
    } else {
        toneKnobIsMoving = false;
    }

}

inline void checkIfModDepthKnobIsMoving(double currentValue) {
    if (((currentValue - previousModDepthKnobValue) > 0.001) or ((currentValue - previousModDepthKnobValue) < -0.001)) {
        previousModDepthKnobValue = currentValue;
        modDepthKnobIsMoving = true;
    } else {
        modDepthKnobIsMoving = false;
    }

}

//These pointers are necessary to speed up the code, otherwise severe crackling occurs.
inline void setAndUpdateGainLeds(const double &w, const double &x, const double &y, const double &z) {
    LED0PtrRed.SetRed(w);
    LED1PtrRed.SetRed(x);
    LED2PtrRed.SetRed(y);
    LED3PtrRed.SetRed(z);
    LED0PtrRed.Update();
    LED1PtrRed.Update();
    LED2PtrRed.Update();
    LED3PtrRed.Update();
}

inline void prepareLeds(const double &w, const double &x, const double &y, const double &z) {
    led1 = w;
    led2 = x;
    led3 = y;
    led4 = z;
}

inline void prepareGenericLed() {
    if(genericLedTimer < genericLedOnTime) {
        ++genericLedTimer;
        prepareLeds(1.,1.,1.,1.);
    } else if (genericLedTimer == genericLedOnTime) {
        genericLedTimer = genericLedOnTime + 1;
        prepareLeds(0.,0.,0.,0.);
    }
}

inline void checkButton() {
    previousButtonState = buttonState;
    hw.tap.Debounce();
    buttonState = hw.tap.Pressed() or !hw.gate.State();
}

inline void checkSwitches() {
    switchState0 = SWITCH0Ptr->Read();
    switchState1 = SWITCH1Ptr->Read();
}

inline void processSwitches() {
    if(switchState0 == 2) {
        if(switchState1 == 1) {
            inputDampHigh = toneKnobZeroLockValue;
            if(((inputDampHigh - previousInputDampHigh) < 0.01) and ((inputDampHigh - previousInputDampHigh) > -0.01)) {
                previousInputDampHigh = inputDampHigh;
                markSettingsDirty();
                reverb.setInputFilterHighCutoffPitch(10. - (10. * inputDampHigh));
                if(toneKnobIsMoving) {
                    toneKnobLedTimer = 0;
                }
                if(toneKnobLedTimer < toneKnobLedOnTime) {
                    ++toneKnobLedTimer;
                    prepareLeds(0.,inputDampHigh,0.,0.);
                } else if (toneKnobLedTimer == toneKnobLedOnTime) {
                    toneKnobLedTimer = toneKnobLedOnTime + 1;
                    prepareLeds(0.,0.,0.,0.);
                }
            }
        } else if (switchState1 == 2) {
            reverbDampHigh = toneKnobZeroLockValue;
            if(((reverbDampHigh - previousReverbDampHigh) < 0.01) and ((reverbDampHigh - previousReverbDampHigh) > -0.01)) {
                previousReverbDampHigh = reverbDampHigh;
                markSettingsDirty();
                reverb.setTankFilterHighCutFrequency(10. - (10. * reverbDampHigh));
                if(toneKnobIsMoving) {
                    toneKnobLedTimer = 0;
                }
                if(toneKnobLedTimer < toneKnobLedOnTime) {
                    ++toneKnobLedTimer;
                    prepareLeds(0.,0.,0.,reverbDampHigh);
                } else if (toneKnobLedTimer == toneKnobLedOnTime) {
                    toneKnobLedTimer = toneKnobLedOnTime + 1;
                    prepareLeds(0.,0.,0.,0.);
                }
            }
        }
    } else if(switchState0 == 1) {
        if(switchState1 == 1) {
            inputDampLow = toneKnobZeroLockValue;
            if(((inputDampLow - previousInputDampLow) < 0.01) and ((inputDampLow - previousInputDampLow) > -0.01)) {
                previousInputDampLow = inputDampLow;
                markSettingsDirty();
                reverb.setInputFilterLowCutoffPitch(inputDampLow * 10.);
                if(toneKnobIsMoving) {
                    toneKnobLedTimer = 0;
                }   
                if(toneKnobLedTimer < toneKnobLedOnTime) {
                    ++toneKnobLedTimer;
                    prepareLeds(inputDampLow,0.,0.,0.);
                } else if (toneKnobLedTimer == toneKnobLedOnTime) {
                    toneKnobLedTimer = toneKnobLedOnTime + 1;
                    prepareLeds(0.,0.,0.,0.);
                }
            }
        } else if (switchState1 == 2) {
            reverbDampLow = toneKnobZeroLockValue;
            if(((reverbDampLow - previousReverbDampLow) < 0.01) and ((reverbDampLow - previousReverbDampLow) > -0.01)) {
                previousReverbDampLow = reverbDampLow;
                markSettingsDirty();
                reverb.setTankFilterLowCutFrequency(reverbDampLow * 10.);
                if(toneKnobIsMoving) {
                    toneKnobLedTimer = 0;
                }
                if(toneKnobLedTimer < toneKnobLedOnTime) {
                    ++toneKnobLedTimer;
                    prepareLeds(0.,0.,reverbDampLow,0.);
                } else if (toneKnobLedTimer == toneKnobLedOnTime) {
                    toneKnobLedTimer = toneKnobLedOnTime + 1;
                    prepareLeds(0.,0.,0.,0.);
                }
            }
        }
    }

    if(switchState1 == 0) {
        switch(switchState0) {
            case 0:
                tempDiffusion = toneKnobZeroLockValue;
                if(((tempDiffusion - diffusion) < 0.01) and ((tempDiffusion - diffusion) > -0.01)) {
                    diffusion = tempDiffusion;
                    markSettingsDirty();
                    if(diffusion == 0.) {
                        if(diffusionEnabled) {
                            diffusionEnabled = false;
                            reverb.enableInputDiffusion(diffusionEnabled);
                        }
                    } else {
                        if(!diffusionEnabled) {
                            diffusionEnabled = true;
                            reverb.enableInputDiffusion(diffusionEnabled);
                        }
                        reverb.setTankDiffusion(diffusion * 0.7);
                    }
                    if(toneKnobIsMoving) {
                        toneKnobLedTimer = 0;
                    }
                    if(toneKnobLedTimer < toneKnobLedOnTime) {
                        ++toneKnobLedTimer;
                        prepareLeds(diffusion,diffusion,diffusion,diffusion);
                    } else if (toneKnobLedTimer == toneKnobLedOnTime) {
                        toneKnobLedTimer = toneKnobLedOnTime + 1;
                        prepareLeds(0.,0.,0.,0.);
                    }
                }
                break;
            case 1:
                tempInputAmplification = toneKnobZeroLockValue;
                if(((tempInputAmplification - inputAmplification) < 0.01) and ((tempInputAmplification - inputAmplification) > -0.01)) {
                    inputAmplification = tempInputAmplification;
                    markSettingsDirty();
                    if(toneKnobIsMoving) {
                        toneKnobLedTimer = 0;
                    }
                    if(toneKnobLedTimer < toneKnobLedOnTime) {
                        ++toneKnobLedTimer;
                        prepareLeds(inputAmplification,inputAmplification,inputAmplification,inputAmplification);
                    } else if (toneKnobLedTimer == toneKnobLedOnTime) {
                        toneKnobLedTimer = toneKnobLedOnTime + 1;
                        prepareLeds(0.,0.,0.,0.);
                    }
                }
                break;
            case 2:
                tempOutputAmplification = toneKnobZeroLockValue;
                if(((tempOutputAmplification - outputAmplification) < 0.01) and ((tempOutputAmplification - outputAmplification) > -0.01)) {
                    outputAmplification = tempOutputAmplification;
                    markSettingsDirty();
                    if(toneKnobIsMoving) {
                        toneKnobLedTimer = 0;
                    }
                    if(toneKnobLedTimer < toneKnobLedOnTime) {
                        ++toneKnobLedTimer;
                        prepareLeds(outputAmplification,outputAmplification,outputAmplification,outputAmplification);
                    } else if (toneKnobLedTimer == toneKnobLedOnTime) {
                        toneKnobLedTimer = toneKnobLedOnTime + 1;
                        prepareLeds(0.,0.,0.,0.);
                    }
                }
                break;
        }
    }
}

// Button has three modes, gain control, buffer clear, freeze.
// Hold down the button for 10 seconds, when the LEDs come on
// press again within one second to confirm mode change. If x gain control mode
// not confirming will lock mod depth to 3.125%. In buffer clear
// mode, a rising edge will trigger the buffers to clear.
// In freeze mode, holding the button will freeze the buffers.
inline void processButton() {
    if(buttonState) {
        if(buttonMode == 0) { 
            if(buttonHoldTimer < 160000) {
                gainModeLedTimer = 0;
            }
        }
        if(buttonHoldTimer == 320000) {
            ++buttonHoldTimer;
            genericLedTimer = 0;
        }
        if(buttonHoldTimer == 352001) {
            ++buttonHoldTimer;
            if(buttonMode == 0) {
                lockedModDepthValue = modDepthValue;
                if(lockModDepthTo3_125_) {
                    shapeSet = false;
                    lockModDepthTo3_125_ = false;
                } else {
                    lockModDepthTo3_125_ = true;
                }
            }
        }
        if(buttonMode == 2) {
            freeze = true;
        } else {
            freeze = false;
        }
        if(!previousButtonState) {
            if(confirmationSequence) {
                if(genericLedTimer < genericLedOnTime) {
                    if(buttonMode == 2) {
                        buttonMode = 0;
                        //saveData();
                    } else {
                        ++buttonMode;
                        //saveData();
                    }
                    confirmationSequence = false;
                } else {
                    // This might be redundant.
                    confirmationSequence = false;
                }
            }
            if(buttonMode == 1) {
                genericLedTimer = 0;
                clear = true;
            }
        }
    } else {
        if(buttonMode == 2) {
            freeze = false;
        }
        if(previousButtonState) {
            if((buttonHoldTimer > 320000) and (buttonHoldTimer < 352000)) {
                confirmationSequence = true;
            }
            if(buttonMode == 0) {
                if (buttonHoldTimer < 8000) {
                    gainModeLedTimer = 0;
                    ++gainMode;
                    saveData();
                }
            }
        }
        buttonHoldTimer = 0;
    }
}

inline void incrementButtonHoldCounterAudioRate() {
    if(buttonState) {
        ++buttonHoldTimer;
    }
}

inline void interpolatingDelayHold() {
    if(holdCount < 192000)
        ++holdCount;
    else
        hold = 1.;
}

inline void processAllParameters() {

    // Putting this here for larger audio block sizes.
    // Quick knob update times means less noise
    hw.ProcessAnalogControls();
    knobValue0 = KNOB0Ptr->Value();
    knobValue1 = KNOB1Ptr->Value();
    knobValue2 = KNOB2Ptr->Value();
    knobValue3 = KNOB3Ptr->Value();
    knobValue4 = KNOB4Ptr->Value();
    knobValue5 = KNOB5Ptr->Value();
    knobValue6 = KNOB6Ptr->Value();

    // If the tone knob is not moving and the mode LEDs are not shining, show audio IO levels on LEDs
    if ((gainModeLedTimer > gainModeLedOnTime) and (!toneKnobIsMoving)) {
        prepareLeds(leftInput * minus20dBGain, rightInput * minus20dBGain, leftOutput, rightOutput);
    }

    // Tone knob parameters smoothly lock to 0 to avoid any clicking when disabling diffusion and unwanted low/high cuts
    toneKnobValue = toneKnobLPF.processLowpass(knobValue2);
    checkIfToneKnobIsMoving(toneKnobValue);
    toneKnobZeroLockValue = toneKnobZeroLockLPF.processLowpass((knobValue2 >= 0.01) * knobValue2);
    if(toneKnobZeroLockValue < 1.0e-030) {
        toneKnobZeroLockValue = 0.;
    }

    // Mix knob locks to zero and one. Mix knob is very susceptible to noise along with pre-delay, mod depth, and time scale
    // These knobs are thus ran through one pole LPFs. It is important these 1 pole LPFs are evaluated at audio rate.
    wet = mixKnobLPF.processLowpass((knobValue0 > 0.99) * 1. + (knobValue0 >= 0.01) * knobValue0 * (knobValue0 <= 0.99));
    dry = 1. - wet;

    // // As with mix, mod speed need not be locked to zero. Mod speed is not succeptible to noise
    reverb.setTankModSpeed(0.5 + (knobValue1 * 100.));

    // Mod depth value also smoothly locks to zero to avoid any clicking
    modDepthValue = modDepthKnobLPF.processLowpass((knobValue3 >= 0.01) * knobValue3);
    if(modDepthValue < 1.0e-030) {
        modDepthValue = 0.;
    }

    // Ability to lock mod depth to the equivalent default 3.125% of VCV rack
    if(lockModDepthTo3_125_) {
        reverb.setTankModShape(0.001 + (modDepthValue * 0.998));
        reverb.setTankModDepth(0.5 + (lockedModDepthValue * 15.5));
    } else {
        if(!shapeSet) {
            reverb.setTankModShape(0.5);
            shapeSet = true;
        }
        reverb.setTankModDepth(modDepthValue * 16.);
    }

    // The decay setting is not succeptible to noise. Exact scaling as x VCV rack.
    // In order for the freeze parameter to not cause any noise, a low pass filter must be applied
    // to the decay param to smoothly move from 100% decay to whatever value is present on the knob.
    if(knobValue4 < 0.01) {
        decay = 0.;
    } else if(knobValue4 > 0.99) {
        decay = 1.;
    } else {
        decay = knobValue4;
    }
    if(freeze) {
        decay = 1.;
    }
    decay = 0.1 + (decay * 0.7999);
    decay = decay + 0.1;
    decay = 1 - decay;
    decay = 1 - (decay * decay);
    decay = decayKnobLPF.processLowpass(decay);
    reverb.setDecay(decay);

    // Time scale is very succeptible to noise. Smoothly locks to zero
    if(knobValue5 < 0.01) {
        timeScale = 0.;
    } else if(knobValue5 > 0.99) {
        timeScale = 1.;
    } else {
        timeScale = knobValue5;
    }
    timeScale = timeScale * timeScale;
    timeScale = 0.0025 + (timeScale * 0.9975);
    timeScale = timeScaleKnobLPF.processLowpass(timeScale) * 4.;
    reverb.setTimeScale(timeScale);
    
    // // Pre-delay knob is smoothly locked to zero and out of all controls is most succeptible to noise
    preDelay = preDelayKnobLPF.processLowpass((knobValue6 >= 0.01) * knobValue6) * 4.;
    if(preDelay < 1.0e-030) {
        preDelay = 0.;
    }
    reverb.setPreDelay(preDelay);

    processSwitches();
}


double modifier = 0.f;



// Is mutating the output this way a mortal sin?
inline void gainControl(double &leftOutput, double &rightOutput) {
    double saturatedLeft = leftOutput;
    double saturatedRight = rightOutput;
    double mix = 1. - ((1. - outputAmplification) * (1. - outputAmplification) * (1. - outputAmplification));
    switch (gainMode) {
    case 0:
        // Regular soft limiter. Rarely clips. Lower limit threshold by turning tone knob up output dynamic setting selected.
        softerLimiterLeft.limit = (0.85 - (outputAmplification * 0.85));
        softerLimiterRight.limit = (0.85 - (outputAmplification * 0.85));
        if(gainModeLedTimer < gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,0.,1.);
        } else if (gainModeLedTimer == gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,0.,0.);
        }
        break;
    case 1:
        // Same clipper as VCV rack. Lower clip threshold by turning tone knob up with output dynamic setting selected.
        softerLimiterLeft.limit = 0.85;
        softerLimiterRight.limit = 0.85;
        hardClipGain = (1. - outputAmplification) * (1. - outputAmplification);
        leftOutput = hardClip(leftOutput);
        rightOutput = hardClip(rightOutput);
        // modifier = (-9.8 / (-40.5 + (40. * outputAmplification))) + 0.758;
        modifier = 1. + ((1. / (hardClipGain + 0.000000001)) * (1.2 - outputAmplification));
        leftOutput *= modifier;
        rightOutput *= modifier;
        if(gainModeLedTimer < gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,1.,0.);
        } else if (gainModeLedTimer == gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,0.,0.);
        }
        break;
    case 2:
        // Same as last but with saturation
        softerLimiterLeft.limit = 0.85;
        softerLimiterRight.limit = 0.85;
        hardClipGain = (1. - outputAmplification) * (1. - outputAmplification);
        leftOutput = hardClip(leftOutput);
        rightOutput = hardClip(rightOutput);
        modifier = 1. + ((1. / (hardClipGain + 0.000000001)) * (1.2 - outputAmplification));
        leftOutput *= modifier;
        rightOutput *= modifier;

        saturatedLeft = leftOutput;
        saturatedRight = rightOutput;
        saturatedLeft *= 1. + outputAmplification * 20.;
        saturatedRight *= 1. + outputAmplification * 20.;
        saturation(saturatedLeft);
        saturation(saturatedRight);
        saturatedLeft *= 1 - 1.6 * mix + 0.83 * mix * mix;
        saturatedRight *= 1 - 1.6 * mix + 0.83 * mix * mix;

        leftOutput = leftOutput * (1. - mix) + saturatedLeft * mix;
        rightOutput = rightOutput * (1. - mix) + saturatedRight * mix;
        limiter.engine.thresholdDb = -24.;
        hardLimiter(leftOutput, rightOutput);
        if(gainModeLedTimer < gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,1.,1.);
        } else if (gainModeLedTimer == gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,0.,0.);
        }
        break;
    case 3:
        // Just saturation. Control gain going into saturation with tone knob output dynamic setting
        softerLimiterLeft.limit = 0.85;
        softerLimiterRight.limit = 0.85;
        saturatedLeft = leftOutput;
        saturatedRight = rightOutput;
        saturatedLeft *= 1. + outputAmplification * 20.;
        saturatedRight *= 1. + outputAmplification * 20.;
        saturation(saturatedLeft);
        saturation(saturatedRight);
        saturatedLeft *= 1 - 1.6 * mix + 0.83 * mix * mix;
        saturatedRight *= 1 - 1.6 * mix + 0.83 * mix * mix;
        // saturatedLeft *= 1. + mix * mix * mix * mix;
        // saturatedRight *= 1. + mix * mix * mix * mix;
        leftOutput = leftOutput * (1. - mix) + saturatedLeft * mix;
        rightOutput = rightOutput * (1. - mix) + saturatedRight * mix;
        limiter.engine.thresholdDb = -24.;
        hardLimiter(leftOutput, rightOutput);
        if(gainModeLedTimer < gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,1.,0.,0.);
        } else if (gainModeLedTimer == gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,0.,0.);
        }
        break;
    case 4:
        // Bogaudio LMTR then stock VCV clip
        softerLimiterLeft.limit = 0.85;
        softerLimiterRight.limit = 0.85;
        limiter.engine.thresholdDb = -24.;
        hardLimiter(leftOutput, rightOutput);
        softerLimiterLeft.limit = 0.85;
        softerLimiterRight.limit = 0.85;
        hardClipGain = (1. - outputAmplification) * (1. - outputAmplification);
        leftOutput = hardClip(leftOutput);
        rightOutput = hardClip(rightOutput);
        // modifier = (-9.8 / (-40.5 + (40. * outputAmplification))) + 0.758;
        modifier = 1. + ((1. / (hardClipGain + 0.000000001)) * (1.2 - outputAmplification));
        leftOutput *= modifier;
        rightOutput *= modifier;
        if(gainModeLedTimer < gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,1.,0.,1.);
        } else if (gainModeLedTimer == gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,0.,0.);
        }
        break;
    case 5:
        // Stock VCV clip then Bogaudio LMTR
        softerLimiterLeft.limit = 0.85;
        softerLimiterRight.limit = 0.85;
        softerLimiterLeft.limit = 0.85;
        softerLimiterRight.limit = 0.85;
        hardClipGain = (1. - outputAmplification) * (1. - outputAmplification);
        leftOutput = hardClip(leftOutput);
        rightOutput = hardClip(rightOutput);
        // modifier = (-9.8 / (-40.5 + (40. * outputAmplification))) + 0.758;
        modifier = 1. + ((1. / (hardClipGain + 0.000000001)) * (1.2 - outputAmplification));
        leftOutput *= modifier;
        rightOutput *= modifier;
        limiter.engine.thresholdDb = -24.;
        hardLimiter(leftOutput, rightOutput);
        if(gainModeLedTimer < gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,1.,1.,0.);
        } else if (gainModeLedTimer == gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,0.,0.);
        }
        break;
    case 6:
        softerLimiterLeft.limit = 0.85;
        softerLimiterRight.limit = 0.85;
        // Foldback distortion. Full wave rectifier that folds back on itself
        foldbackDistortion(leftOutput, 1. - outputAmplification);
        foldbackDistortion(rightOutput, 1. - outputAmplification);
        limiter.engine.thresholdDb = -24.;
        hardLimiter(leftOutput, rightOutput);
        if(gainModeLedTimer < gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,1.,1.,1.);
        } else if (gainModeLedTimer == gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,0.,0.);
        }
        break;
    case 7:
        softerLimiterLeft.limit = 0.85;
        softerLimiterRight.limit = 0.85;
        saturatedLeft = leftOutput;
        saturatedRight = rightOutput;
        // Output to zero once past threshold. Simulates ripped speaker
        rippedSpeakerLeft(saturatedLeft, (2. + 2. * outputAmplification * outputAmplification - 4. * outputAmplification));
        rippedSpeakerRight(saturatedRight, (2. + 2. * outputAmplification * outputAmplification - 4. * outputAmplification));
        mix *= 2;
        if(mix > 1.){
            mix = 1.;
        }
        leftOutput = leftOutput * (1. - mix) + saturatedLeft * mix;
        rightOutput = rightOutput * (1. - mix) + saturatedRight * mix;
        limiter.engine.thresholdDb = -24.;
        hardLimiter(leftOutput, rightOutput);
        if(gainModeLedTimer < gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(1.,0.,0.,0.);
        } else if (gainModeLedTimer == gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,0.,0.);
        }
        break;
    case 8:
        // Same as last but saturation before ripped speaker
        softerLimiterLeft.limit = 0.85;
        softerLimiterRight.limit = 0.85;
        saturatedLeft = leftOutput;
        saturatedRight = rightOutput;
        rippedSpeakerLeft(saturatedLeft, (2. + 2. * outputAmplification * outputAmplification - 4. * outputAmplification));
        rippedSpeakerRight(saturatedRight, (2. + 2. * outputAmplification * outputAmplification - 4. * outputAmplification));
        saturatedLeft *= 1. + outputAmplification * 15.;
        saturatedRight *= 1. + outputAmplification * 15.;
        saturation(saturatedLeft);
        saturation(saturatedRight);
        saturatedLeft *= 1 - 1.6 * mix + 0.83 * mix * mix;
        saturatedRight *= 1 - 1.6 * mix + 0.83 * mix * mix;
        mix *= 2;
        if(mix > 1.){
            mix = 1.;
        }
        leftOutput = leftOutput * (1. - mix) + saturatedLeft * mix;
        rightOutput = rightOutput * (1. - mix) + saturatedRight * mix;
        limiter.engine.thresholdDb = -24.;
        hardLimiter(leftOutput, rightOutput);
        if(gainModeLedTimer < gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(1.,0.,0.,1.);
        } else if (gainModeLedTimer == gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,0.,0.);
        }
        break;
    case 9:
        // Same as last but in addition to saturation there is also a hard clipper
        softerLimiterLeft.limit = 0.85;
        softerLimiterRight.limit = 0.85;
        hardClipGain = (1. - outputAmplification) * (1. - outputAmplification) + 0.1;
        leftOutput = hardClip(leftOutput);
        rightOutput = hardClip(rightOutput);
        modifier = 1. + ((1. / (hardClipGain - 0.1 + 0.000000001)) * (1.2 - outputAmplification));
        leftOutput *= modifier;
        rightOutput *= modifier;

        saturatedLeft = leftOutput;
        saturatedRight = rightOutput;
        
        rippedSpeakerLeft(saturatedLeft, (2. + 2. * outputAmplification * outputAmplification - 4. * outputAmplification));
        rippedSpeakerRight(saturatedRight, (2. + 2. * outputAmplification * outputAmplification - 4. * outputAmplification));
        saturatedLeft *= 1. + outputAmplification * 15.;
        saturatedRight *= 1. + outputAmplification * 15.;
        saturation(saturatedLeft);
        saturation(saturatedRight);
        saturatedLeft *= 1 - 1.6 * mix + 0.83 * mix * mix;
        saturatedRight *= 1 - 1.6 * mix + 0.83 * mix * mix;

        mix *= 2;
        if(mix > 1.){
            mix = 1.;
        }
        leftOutput = leftOutput * (1. - mix) + saturatedLeft * mix;
        rightOutput = rightOutput * (1. - mix) + saturatedRight * mix;
        
        limiter.engine.thresholdDb = -24.;
        hardLimiter(leftOutput, rightOutput);
        if(gainModeLedTimer < gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(1.,0.,1.,0.);
        } else if (gainModeLedTimer == gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,0.,0.);
        }
        break;
    case 10:
        // Last one is Bogaudio LMTR followed by the ripped speaker
        softerLimiterLeft.limit = 0.85;
        softerLimiterRight.limit = 0.85;
        limiter.engine.thresholdDb = -30.;
        hardLimiter(leftOutput, rightOutput);
        saturatedLeft = leftOutput;
        saturatedRight = rightOutput;
        rippedSpeakerLeft(saturatedLeft, (2. + 2. * outputAmplification * outputAmplification - 4. * outputAmplification));
        rippedSpeakerRight(saturatedRight, (2. + 2. * outputAmplification * outputAmplification - 4. * outputAmplification));
        mix *= 2;
        if(mix > 1.){
            mix = 1.;
        }
        leftOutput = leftOutput * (1. - mix) + saturatedLeft * mix;
        rightOutput = rightOutput * (1. - mix) + saturatedRight * mix;
        if(gainModeLedTimer < gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(1.,0.,1.,1.);
        } else if (gainModeLedTimer == gainModeLedOnTime) {
            ++gainModeLedTimer;
            prepareLeds(0.,0.,0.,0.);
        }
        break;
    case 11:
        gainMode = 0;
        break;
    }
    softLimiter(leftOutput, rightOutput);
}

struct PopFilter {
	double tmp = 0.;

	PopFilter() {
        inline double processLowpass(const double &x);
    }

    double processLowpass(const double &x) {
        tmp = 0.01 * x + 0.99 * tmp;
        return tmp;
    }
};

PopFilter clearPopFilter;

inline void prepareToClear() {
    if(clear) {
        triggerClear = true;
        clear = false;
    }

    clearPopCancelValue = clearPopFilter.processLowpass(!triggerClear);

    if(clearPopCancelValue > 1 - 1e-30) {
        clearPopFilter.tmp = 1.;
        clearPopCancelValue = 1.;
    }
}

//unsigned int counter = 0;
void AudioCallback(AudioHandle::InputBuffer x,
              AudioHandle::OutputBuffer out,
              size_t size)
{
    for(size_t i = 0; i < size; i += 1) { 
        reverb.freeze(freeze);

        processAllParameters();

        incrementButtonHoldCounterAudioRate();
        saveCounterAudioRate();

        interpolatingDelayHold();

        prepareGenericLed();

        prepareToClear();
        
        leftInput = hardLimit100_(x[0][i]) * 10.;
        rightInput = hardLimit100_(x[1][i]) * 10.;

        reverb.process(leftInput * minus18dBGain * minus20dBGain * (1.0 + inputAmplification * 7.) * clearPopCancelValue,
                    rightInput * minus18dBGain * minus20dBGain * (1.0 + inputAmplification * 7.) * clearPopCancelValue);

        leftOutput = ((leftInput * dry * 0.1) + 
                    (reverb.getLeftOutput() * wet * clearPopCancelValue));
        rightOutput = ((rightInput * dry * 0.1) + 
                    (reverb.getRightOutput() * wet * clearPopCancelValue));

        gainControl(leftOutput, rightOutput);

        out[0][i] = leftOutput;
        out[1][i] = rightOutput;
    }
};

uint32_t testValue = 0;
double maxLoad = 0.;

int main(void)
{
	hw.Init(true);

    limiter.init();

    softerLimiterLeft.configure(32000);
    softerLimiterRight.configure(32000);

    // LEDs indicate we are starting up
    hw.leds[0].Set(1, 0, 0);
    hw.leds[1].Set(1, 0, 0);
    hw.leds[2].Set(1, 0, 0);
    hw.leds[3].Set(1, 0, 0);
    hw.UpdateLeds();

    for(int i = 0; i < 50; i++) {
        for(int j = 0; j < 144000; j++) {
            sdramData[i][j] = 0.;
        }
    }

    // Setup default settings and load saved data
    Settings defaults;
    defaults.gainMode = 0;
    defaults.version = settingsVersion;
    defaults.inputDampLow = 0.;
    defaults.inputDampHigh = 0.;
    defaults.reverbDampLow = 0.;
    defaults.reverbDampHigh = 0.;
    defaults.diffusion = 1.;
    defaults.inputAmplification = 0.;
    defaults.outputAmplification = 0.;
    storage.Init(defaults);
    loadData();

    reverb.setSampleRate(32000);

    reverb.setTimeScale(1.007500);
    reverb.setPreDelay(0.000000);

    reverb.setInputFilterLowCutoffPitch(10. * inputDampLow);
    reverb.setInputFilterHighCutoffPitch(10. - (10. * inputDampHigh));
    reverb.enableInputDiffusion(diffusionEnabled);
    reverb.setDecay(0.877465);
    reverb.setTankDiffusion(diffusion * 0.7);
    reverb.setTankFilterLowCutFrequency(10. * reverbDampLow);
    reverb.setTankFilterHighCutFrequency(10. - (10. * reverbDampHigh));
    reverb.setTankModSpeed(1.0);
    reverb.setTankModDepth(0.5);
    reverb.setTankModShape(0.5);

    hw.SetAudioBlockSize(32);
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_32KHZ);
    hw.seed.audio_handle.SetPostGain(1.0);
    hw.seed.audio_handle.SetOutputCompensation(1.0);

    hw.knobs[0].SetCoeff(0.001);
    hw.knobs[1].SetCoeff(0.01);
    hw.knobs[2].SetCoeff(0.01);
    hw.knobs[3].SetCoeff(0.001);
    hw.knobs[4].SetCoeff(0.01);
    hw.knobs[5].SetCoeff(0.01);
    hw.knobs[6].SetCoeff(0.001);
    
    hw.StartAudio(AudioCallback);

    hw.StartAdc();

	// LEDs indicate that we are ready to go
    hw.leds[0].Set(1, 0, 0);
    hw.leds[1].Set(1, 1, 0);
    hw.leds[2].Set(0, 1, 0);
    hw.leds[3].Set(0, 0, 1);
    hw.UpdateLeds();
    System::Delay(500);
    hw.leds[0].Set(0, 0, 0);
    hw.leds[1].Set(0, 0, 0);
    hw.leds[2].Set(0, 0, 0);
    hw.leds[3].Set(0, 0, 0);
    hw.UpdateLeds();

	while(1) {
        checkSwitches();
        // Process switches occurs at audio rate x the callback
        checkButton();
        processButton();
        if(saveTrigger) {
            saveData();
        }
        // The button LED counter occurs at audio rate
        setAndUpdateGainLeds(led1, led2, led3, led4);

        if(clearPopCancelValue < 1e-30) {
            if(triggerClear) {
                clearPopFilter.tmp = 0.;
                clearPopCancelValue = 0.;
                reverb.clear();
                triggerClear = false;
            }
        }
        // Clear buffers. Fingers crossed it works.
        // if(triggerClear) {
        //     if(clearPopCancelValue < 0.0001) {
        //         reverb.clear();
        //     }
            
        // }
    }
}
