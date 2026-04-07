# Builds tests
{ pkgs, common, build }:

pkgs.stdenv.mkDerivation {
  pname = "${common.pname}-tests";
  version = common.version;

  inherit (build) src;
  inherit (common) buildInputs meta;

  nativeBuildInputs = common.nativeBuildInputs
    ++ pkgs.lib.optionals pkgs.stdenv.isDarwin [ pkgs.darwin.cctools ]
    ++ pkgs.lib.optionals pkgs.stdenv.isLinux [ pkgs.patchelf ];

  cmakeFlags = common.cmakeFlags;

  configurePhase = ''
    runHook preConfigure

    cp -r ${build}/* .
    chmod -R u+w .

    cmake -B build -S ${build.src} \
      -GNinja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=$out

    runHook postConfigure
  '';

  buildPhase = ''
    runHook preBuild

    cd build
    ninja process_stats_tests

    runHook postBuild
  '';

  installPhase = ''
    runHook preInstall

    mkdir -p $out/bin
    cp bin/process_stats_tests $out/bin/

    mkdir -p $out/lib
    cp -r lib/* $out/lib/ || true

    ${pkgs.lib.optionalString pkgs.stdenv.isDarwin ''
      install_name_tool \
        -change @rpath/libprocess_stats.dylib $out/lib/libprocess_stats.dylib \
        $out/bin/process_stats_tests || true
    ''}

    ${pkgs.lib.optionalString pkgs.stdenv.isLinux ''
      patchelf --set-rpath "$out/lib:${pkgs.gtest}/lib:${pkgs.stdenv.cc.cc.lib}/lib" $out/bin/process_stats_tests || true
    ''}

    runHook postInstall
  '';
}
