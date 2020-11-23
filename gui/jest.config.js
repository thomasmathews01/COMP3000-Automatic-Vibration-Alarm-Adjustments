module.exports = {
    preset: 'ts-jest',
    testEnvironment: 'node',
    roots: [
        "<rootDir>/UnitTests"
    ],
    testMatch: [
        "**/__tests__/**/*.+(ts|tsx|js)",
        "**/?(*.)+(spec|test).+(ts|tsx|js)"
    ],
    setupFilesAfterEnv: [
        "<rootDir>/enzyme.js"
    ],
    testEnvironment: 'jsdom',
    globals: {
        "ts-jest": {
            tsConfig: "jest.tsconfig.json"
        }
    },
    reporters: ["default"]
};