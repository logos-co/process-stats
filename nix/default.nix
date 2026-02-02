# Common build configuration shared across all packages
{ pkgs }:

{
  pname = "process-stats";
  version = "0.1.0";
  
  # Common native build inputs
  nativeBuildInputs = [ 
    pkgs.cmake 
    pkgs.ninja 
    pkgs.pkg-config
    pkgs.qt6.wrapQtAppsNoGuiHook
  ];
  
  # Common runtime dependencies
  buildInputs = [ 
    pkgs.qt6.qtbase 
    pkgs.gtest
  ];
  
  # Common CMake flags
  cmakeFlags = [ 
    "-GNinja"
  ];
  
  # Metadata
  meta = with pkgs.lib; {
    description = "Process statistics library for monitoring CPU and memory usage";
    platforms = platforms.unix;
  };
}
