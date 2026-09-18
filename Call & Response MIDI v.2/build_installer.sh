#!/bin/bash
# ==============================================================================
# Call & Response MIDI v2.0.0 - Universal macOS Installer Builder
# Viral Samples
# Compatible with macOS Mojave 10.14+ (Intel) and Apple Silicon (M1-M4)
# ==============================================================================
set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALLER_DIR="$PROJECT_ROOT/installer"
BUILD_DIR="$PROJECT_ROOT/build"
STAGING_DIR="$INSTALLER_DIR/staging"
PACKAGES_DIR="$INSTALLER_DIR/packages"
RESOURCES_DIR="$PROJECT_ROOT/resources"
SCRIPTS_DIR="$PROJECT_ROOT/scripts"
DOC_DIR="$PROJECT_ROOT/Documentation"
DISTRIBUTION_XML="$PROJECT_ROOT/distribution.xml"
TARGET_RELEASE_DIR="$PROJECT_ROOT/Viral Samples - Call & Response MIDI v2.0.0 (MAC:WIN)"

VERSION="2.0.0"
PKG_NAME="Call & Response MIDI v${VERSION} Installer.pkg"
FINAL_PKG="$PROJECT_ROOT/$PKG_NAME"

echo "======================================================================"
echo " Building Call & Response MIDI v${VERSION} Universal macOS Installer "
echo " Mojave 10.14+ Intel (x86_64) & Apple Silicon (arm64) Compatible"
echo "======================================================================"

# 1. Clean staging & package dirs
echo "==> Preparing staging directories..."
rm -rf "$STAGING_DIR" "$PACKAGES_DIR"
mkdir -p "$STAGING_DIR/vst3" "$STAGING_DIR/au" "$STAGING_DIR/doc" "$PACKAGES_DIR" "$RESOURCES_DIR" "$TARGET_RELEASE_DIR"

VST3_SRC="$BUILD_DIR/CallAndResponseMIDI_artefacts/Release/VST3/Call & Response MIDI.vst3"
AU_SRC="$BUILD_DIR/CallAndResponseMIDI_artefacts/Release/AU/Call & Response MIDI.component"

if [ ! -d "$VST3_SRC" ]; then
    echo "ERROR: VST3 bundle not found at: $VST3_SRC"
    exit 1
fi

if [ ! -d "$AU_SRC" ]; then
    echo "ERROR: AU bundle not found at: $AU_SRC"
    exit 1
fi

# Verify architectures
echo "==> Verifying binary architectures..."
lipo -info "$VST3_SRC/Contents/MacOS/Call & Response MIDI"
lipo -info "$AU_SRC/Contents/MacOS/Call & Response MIDI"

# 2. Copy binaries and documentation to staging
echo "==> Staging VST3 and AU bundles..."
cp -R "$VST3_SRC" "$STAGING_DIR/vst3/"
cp -R "$AU_SRC" "$STAGING_DIR/au/"
echo "==> Staging manuals and documentation (PDF, HTML, MD, TXT)..."
cp "$DOC_DIR"/*.pdf "$STAGING_DIR/doc/" 2>/dev/null || true
cp "$DOC_DIR"/*.html "$STAGING_DIR/doc/" 2>/dev/null || true
cp "$DOC_DIR"/*.md "$STAGING_DIR/doc/" 2>/dev/null || true
cp "$DOC_DIR"/*.txt "$STAGING_DIR/doc/" 2>/dev/null || true

# 3. Dequarantine and ad-hoc sign staged binaries
echo "==> Dequarantining and codesigning staged plugins..."
xattr -cr "$STAGING_DIR/vst3/Call & Response MIDI.vst3"
xattr -cr "$STAGING_DIR/au/Call & Response MIDI.component"
codesign --force --deep -s - "$STAGING_DIR/vst3/Call & Response MIDI.vst3"
codesign --force --deep -s - "$STAGING_DIR/au/Call & Response MIDI.component"

# 4. Make postinstall executable
chmod +x "$SCRIPTS_DIR/postinstall"

# 5. Build component packages
echo "==> Building component packages with pkgbuild..."
pkgbuild --root "$STAGING_DIR/vst3" \
         --install-location "/Library/Audio/Plug-Ins/VST3" \
         --identifier "com.viralsamples.pkg.callresponsemidi.vst3" \
         --version "$VERSION" \
         --scripts "$SCRIPTS_DIR" \
         "$PACKAGES_DIR/vst3.pkg"

pkgbuild --root "$STAGING_DIR/au" \
         --install-location "/Library/Audio/Plug-Ins/Components" \
         --identifier "com.viralsamples.pkg.callresponsemidi.au" \
         --version "$VERSION" \
         "$PACKAGES_DIR/au.pkg"

pkgbuild --root "$STAGING_DIR/doc" \
         --install-location "/Library/Audio/Documentation/Viral Samples/Call & Response MIDI" \
         --identifier "com.viralsamples.pkg.callresponsemidi.doc" \
         --version "$VERSION" \
         "$PACKAGES_DIR/doc.pkg"

# 6. Build final product distribution package
echo "==> Synthesizing and building distribution product with productbuild..."
productbuild --distribution "$DISTRIBUTION_XML" \
             --package-path "$PACKAGES_DIR" \
             --resources "$RESOURCES_DIR" \
             "$FINAL_PKG"

# 7. Dequarantine and ad-hoc sign installer package
echo "==> Dequarantining and codesigning final installer .pkg..."
xattr -cr "$FINAL_PKG"
codesign --force -s - "$FINAL_PKG"
codesign -vvv --strict "$FINAL_PKG"

# Keep synchronized copies
mkdir -p "$INSTALLER_DIR"
cp "$FINAL_PKG" "$INSTALLER_DIR/$PKG_NAME"
cp "$FINAL_PKG" "$TARGET_RELEASE_DIR/$PKG_NAME"

echo "======================================================================"
echo " SUCCESS: Universal Installer generated at:"
echo "   $FINAL_PKG"
echo " And placed in release directory:"
echo "   $TARGET_RELEASE_DIR/$PKG_NAME"
echo "======================================================================"
