# Installs the process-stats headers
{ pkgs, common, src }:

pkgs.stdenv.mkDerivation {
  pname = "${common.pname}-headers";
  version = common.version;
  
  inherit src;
  inherit (common) meta;
  
  # No build phase needed, just install headers
  dontBuild = true;
  dontConfigure = true;
  
  installPhase = ''
    runHook preInstall
    
    # Install headers in process_stats subdirectory
    mkdir -p $out/include/process_stats
    
    if [ -f src/process_stats.h ]; then
      cp src/process_stats.h $out/include/process_stats/
    fi
    
    runHook postInstall
  '';
}
