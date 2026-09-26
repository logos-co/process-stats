# Common build configuration shared across all packages
{ pkgs }:

let
  canRunTests = pkgs.stdenv.buildPlatform.canExecute pkgs.stdenv.hostPlatform;
in
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
  ]
  # gtest_discover_tests runs the freshly linked binary on the build host,
  # which cannot execute a cross build. No tests under cross, so no gtest either.
  ++ pkgs.lib.optional canRunTests pkgs.gtest;

  cmakeFlags = [
    "-GNinja"
  ] ++ pkgs.lib.optional (!canRunTests) "-DPROCESS_STATS_BUILD_TESTS=OFF";

  meta = with pkgs.lib; {
    description = "Process statistics library for monitoring CPU and memory usage";
    platforms = platforms.unix ++ platforms.windows;
  };
}
