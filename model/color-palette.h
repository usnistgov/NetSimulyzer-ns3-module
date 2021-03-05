
/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 *
 * Author: Evan Black <evan.black@nist.gov>
 */

#ifndef COLOR_PALETTE_H
#define COLOR_PALETTE_H

#include "color.h"

namespace ns3::netsimulyzer {

const Color3 RED{245u, 61u, 0u};
const Color3 DARK_RED{204u, 51u, 0u};

const Color3 GREEN{9u, 232u, 94u};
const Color3 DARK_GREEN{6u, 177u, 72u};

const Color3 BLUE{37u, 137u, 189u};
const Color3 DARK_BLUE{27u, 98u, 136u};

const Color3 ORANGE{255u, 167u, 51u};
const Color3 DARK_ORANGE{245u, 139u, 0u};

const Color3 YELLOW{255U, 227u, 71u};
const Color3 DARK_YELLOW{255U, 218u, 10u};

const Color3 PURPLE{120u, 41u, 163u};
const Color3 DARK_PURPLE{84u, 29u, 114u};

const Color3 PINK{255u, 92u, 176u};
const Color3 DARK_PINK{255u, 51u, 156u};

const Color3 BLACK{7u, 6u, 0u};

const Color3 WHITE{255u, 255u, 255u};

// ----- Values -----

const Color3Value RED_VALUE{RED};
const Color3Value DARK_RED_VALUE{DARK_RED};

const Color3Value GREEN_VALUE{GREEN};
const Color3Value DARK_GREEN_VALUE{DARK_GREEN};

const Color3Value BLUE_VALUE{BLUE};
const Color3Value DARK_BLUE_VALUE{DARK_BLUE};

const Color3Value ORANGE_VALUE{ORANGE};
const Color3Value DARK_ORANGE_VALUE{DARK_ORANGE};

const Color3Value YELLOW_VALUE{YELLOW};
const Color3Value DARK_YELLOW_VALUE{DARK_YELLOW};

const Color3Value PURPLE_VALUE{PURPLE};
const Color3Value DARK_PURPLE_VALUE{DARK_PURPLE};

const Color3Value PINK_VALUE{PINK};
const Color3Value DARK_PINK_VALUE{DARK_PINK};

const Color3Value BLACK_VALUE{BLACK};

const Color3Value WHITE_VALUE{WHITE};
}

#endif //COLOR_PALETTE_H