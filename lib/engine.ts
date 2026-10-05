// TypeScript mirrors of the WarrantyIQ C++ Statistical & Reliability Engine

export interface WarrantyClaim {
  id: string;
  vin: string;
  componentId: string;
  componentName: string;
  failureMode: string;
  mileageHours: number;
  repairCostUsd: number;
  dealerCode: string;
  claimDate: string;
  telemetryValidated: boolean;
  anomalyScore: number; // 0.0 to 1.0 (higher = fraud/irregularity risk)
  status: 'APPROVED' | 'FLAGGED_AUDIT' | 'UNDER_REVIEW';
}

export interface WeibullCurvePoint {
  t: number;
  reliability: number;   // R(t)
  failureProb: number;   // F(t)
  hazardRate: number;    // h(t)
}

export interface ComponentCluster {
  clusterId: number;
  rootComponent: string;
  subsystemName: string;
  memberParts: string[];
  totalExposureUsd: number;
  claimCount: number;
  correlatedFailureReason: string;
}

export interface ReserveForecastMonth {
  month: string;
  projectedClaimsCount: number;
  liabilityReserveUsd: number;
  sparePartsBufferUnits: number;
}

// Gamma function Lanczos approximation for MTTF calculation
export function gamma(z: number): number {
  const g = 7;
  const C = [
    0.99999999999980993,
    676.5203681218851,
    -1259.1392167224028,
    771.32342877765313,
    -176.61502916214059,
    12.507343278686905,
    -0.13857109526572012,
    9.9843695780195716e-6,
    1.5056327351493116e-7
  ];
  if (z < 0.5) return Math.PI / (Math.sin(Math.PI * z) * gamma(1 - z));
  z -= 1;
  let x = C[0];
  for (let i = 1; i < g + 2; i++) {
    x += C[i] / (z + i);
  }
  const t = z + g + 0.5;
  return Math.sqrt(2 * Math.PI) * Math.pow(t, z + 0.5) * Math.exp(-t) * x;
}

export function computeWeibullMetrics(beta: number, eta: number) {
  const mttf = eta * gamma(1 + 1 / beta);
  const b10Life = eta * Math.pow(-Math.log(0.90), 1 / beta);

  // Generate 25 points along the time curve (from 0 to 2.5 * eta)
  const maxT = eta * 2.2;
  const points: WeibullCurvePoint[] = [];

  for (let i = 1; i <= 25; i++) {
    const t = (maxT / 25) * i;
    const rel = Math.exp(-Math.pow(t / eta, beta));
    const fail = 1 - rel;
    const haz = (beta / eta) * Math.pow(t / eta, beta - 1);
    points.push({
      t: Math.round(t),
      reliability: Number(rel.toFixed(3)),
      failureProb: Number(fail.toFixed(3)),
      hazardRate: Number(haz.toFixed(5)),
    });
  }

  return {
    mttf: Math.round(mttf),
    b10Life: Math.round(b10Life),
    points,
  };
}

export const SAMPLE_CLAIMS: WarrantyClaim[] = [
  {
    id: "CLM-90214",
    vin: "1HGCR2F83HA09124",
    componentId: "ALT-48V-HEV",
    componentName: "48V Belt-Driven Starter Alternator",
    failureMode: "Thermal Diode Breakdown",
    mileageHours: 1240,
    repairCostUsd: 1450,
    dealerCode: "DLR-IL-108",
    claimDate: "2026-09-18",
    telemetryValidated: true,
    anomalyScore: 0.12,
    status: 'APPROVED',
  },
  {
    id: "CLM-90215",
    vin: "1HGCR2F83HA09419",
    componentId: "BAT-AGM-48V",
    componentName: "48V Auxiliary Lithium-AGM Pack",
    failureMode: "Secondary Over-Voltage Degradation",
    mileageHours: 1255,
    repairCostUsd: 2100,
    dealerCode: "DLR-IL-108",
    claimDate: "2026-09-19",
    telemetryValidated: true,
    anomalyScore: 0.18,
    status: 'APPROVED',
  },
  {
    id: "CLM-90228",
    vin: "3FA6P0H78HR19421",
    componentId: "TRB-VGT-GEN2",
    componentName: "Variable-Geometry Turbocharger",
    failureMode: "Bearing Carbonization / Oil Starvation",
    mileageHours: 890,
    repairCostUsd: 3400,
    dealerCode: "DLR-TX-402",
    claimDate: "2026-09-24",
    telemetryValidated: false,
    anomalyScore: 0.88,
    status: 'FLAGGED_AUDIT',
  },
  {
    id: "CLM-90233",
    vin: "WAUZZZF27NA01948",
    componentId: "VLT-REG-01",
    componentName: "Micro-Hybrid Voltage Regulator",
    failureMode: "Field Excitation Coil Open Circuit",
    mileageHours: 1310,
    repairCostUsd: 620,
    dealerCode: "DLR-OH-305",
    claimDate: "2026-09-28",
    telemetryValidated: true,
    anomalyScore: 0.08,
    status: 'APPROVED',
  },
  {
    id: "CLM-90245",
    vin: "2C3CDXBG8JH10943",
    componentId: "INJ-DIR-HP",
    componentName: "High-Pressure Direct Fuel Injector",
    failureMode: "Piezo Crystal Delamination",
    mileageHours: 2100,
    repairCostUsd: 1850,
    dealerCode: "DLR-CA-701",
    claimDate: "2026-10-01",
    telemetryValidated: true,
    anomalyScore: 0.22,
    status: 'APPROVED',
  },
  {
    id: "CLM-90251",
    vin: "5N1ED28E1FC98124",
    componentId: "TRB-VGT-GEN2",
    componentName: "Variable-Geometry Turbocharger",
    failureMode: "Wastegate Actuator Seizure",
    mileageHours: 320,
    repairCostUsd: 3850,
    dealerCode: "DLR-FL-904",
    claimDate: "2026-10-02",
    telemetryValidated: false,
    anomalyScore: 0.94,
    status: 'FLAGGED_AUDIT',
  },
];

export const SAMPLE_CLUSTERS: ComponentCluster[] = [
  {
    clusterId: 1,
    rootComponent: "ALT-48V-HEV",
    subsystemName: "48V Mild-Hybrid Powertrain & Charging Subsystem",
    memberParts: ["ALT-48V-HEV", "BAT-AGM-48V", "VLT-REG-01"],
    totalExposureUsd: 417000,
    claimCount: 142,
    correlatedFailureReason: "Alternator thermal bridge breakdown causes transient high-voltage ripple, damaging downstream AGM battery BMS and voltage regulators within 50 operating hours.",
  },
  {
    clusterId: 2,
    rootComponent: "OIL-SNS-P",
    subsystemName: "Forced Induction Lubrication & Turbocharger Subsystem",
    memberParts: ["TRB-VGT-GEN2", "OIL-SNS-P", "COOL-RTN-LINE"],
    totalExposureUsd: 685000,
    claimCount: 98,
    correlatedFailureReason: "Erratic oil pressure transducer latency fails to signal low lubrication head pressure, starving variable-geometry turbocharger thrust bearings under boost.",
  },
  {
    clusterId: 3,
    rootComponent: "INJ-DIR-HP",
    subsystemName: "Direct Injection Fuel Rails",
    memberParts: ["INJ-DIR-HP", "PMP-HP-HPFP"],
    totalExposureUsd: 290000,
    claimCount: 64,
    correlatedFailureReason: "High-pressure fuel pump cam follower wear introduces metallic micro-debris into common rail injectors, causing nozzle seating failure.",
  },
];

export const DEFAULT_FORECAST: ReserveForecastMonth[] = [
  { month: "Nov 2026", projectedClaimsCount: 185, liabilityReserveUsd: 384000, sparePartsBufferUnits: 230 },
  { month: "Dec 2026", projectedClaimsCount: 210, liabilityReserveUsd: 436000, sparePartsBufferUnits: 260 },
  { month: "Jan 2027", projectedClaimsCount: 245, liabilityReserveUsd: 512000, sparePartsBufferUnits: 310 },
  { month: "Feb 2027", projectedClaimsCount: 220, liabilityReserveUsd: 462000, sparePartsBufferUnits: 280 },
  { month: "Mar 2027", projectedClaimsCount: 195, liabilityReserveUsd: 410000, sparePartsBufferUnits: 245 },
  { month: "Apr 2027", projectedClaimsCount: 175, liabilityReserveUsd: 368000, sparePartsBufferUnits: 220 },
];
