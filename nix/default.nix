# Common build configuration shared across all packages
{ pkgs }:

{
  pname = "process-stats";
  version = "0.1.0";

  nativeBuildInputs = [
    pkgs.cmake
    pkgs.ninja
    pkgs.pkg-config
  ];

  buildInputs = [
    pkgs.nlohmann_json
    pkgs.gtest
  ];

  cmakeFlags = [
    "-GNinja"
  ];

  meta = with pkgs.lib; {
    description = "Process statistics library for monitoring CPU and memory usage";
    platforms = platforms.unix;
  };
}
