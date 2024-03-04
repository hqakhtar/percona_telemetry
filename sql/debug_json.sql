--
-- json debugging
--

CREATE OR REPLACE FUNCTION read_json_file()
RETURNS text
AS $$
DECLARE
    file_path text;
    system_id text;
    pg_version text;
    file_content text;
BEGIN
    SELECT output_file_name INTO file_path FROM percona_telemetry_status();
    SELECT system_identifier::TEXT INTO system_id FROM pg_control_system();
    SELECT version() INTO pg_version;

    file_content := pg_read_file(file_path);

    SELECT REPLACE(file_content, system_id, '<SYSTEM IDENTIFIER>') INTO file_content;
    SELECT REPLACE(file_content, pg_version, '<PG VERSION>') INTO file_content;

    -- If we have more than 6 consecutive digits, it's file size unlike to be an OID
    SELECT regexp_replace(file_content, '\d{6,}', '<DB SIZE>', 'g') INTO file_content;

    RETURN file_content;
END;
$$ LANGUAGE plpgsql;

-- Let's sleep for a few seconds to ensure that leader has
-- generated the json file.
SELECT pg_sleep(3);

CREATE EXTENSION percona_telemetry;
SELECT read_json_file();
DROP EXTENSION percona_telemetry;

DROP FUNCTION read_json_file;