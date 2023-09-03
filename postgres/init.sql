
-- Create the RUN table
CREATE TABLE run (
    id              SERIAL          PRIMARY KEY,
    timestamp       TIMESTAMP       DEFAULT current_timestamp,
    interval        SMALLINT,
    oscillation     VARCHAR(50)
);

-- Baseline Measurements
CREATE TABLE baseline (
    id              SERIAL          PRIMARY KEY,
    timestamp       TIMESTAMP       DEFAULT current_timestamp, -- consider not adding here (same as run??)
    run             INT             REFERENCES run(id),
    currencyPair    VARCHAR(8),
    bid             VARCHAR(50),
    ask             VARCHAR(50)
);

-- Stores oscillations which deviate over the 'oscillation' from the initial tick information
CREATE TABLE tick (
    id              SERIAL          PRIMARY KEY,
    baseline        INT             REFERENCES baseline(id),
    timestamp       TIMESTAMP       DEFAULT current_timestamp,
    rate            VARCHAR(50),
    side            CHAR
);
