/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    "./app/**/*.{js,ts,jsx,tsx,mdx}",
    "./components/**/*.{js,ts,jsx,tsx,mdx}",
  ],
  theme: {
    extend: {
      colors: {
        warranty: {
          dark: "#090A0F",
          card: "#12141F",
          border: "#1E2235",
          accent: "#0066FF",
          cyan: "#00E5FF",
          purple: "#9933FF",
          amber: "#FFB000",
          rose: "#FF3366",
          emerald: "#00E599",
        },
      },
    },
  },
  plugins: [],
};
