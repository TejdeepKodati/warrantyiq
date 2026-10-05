import './globals.css';
import type { Metadata } from 'next';

export const metadata: Metadata = {
  title: 'WarrantyIQ | Predictive Warranty & After-Market Telemetry Intelligence Platform',
  description: 'Enterprise after-market analytics engine featuring C++ Weibull MLE, P-Square streaming quantile anomaly detection, and DSU failure clustering for warranty reserve optimization.',
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <body className="bg-[#090A0F] text-[#E0E3EC] min-h-screen antialiased selection:bg-blue-600 selection:text-white">
        {children}
      </body>
    </html>
  );
}
