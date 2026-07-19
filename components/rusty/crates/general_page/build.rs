fn main() {
    slint_build::compile_with_config(
        "ui/window.slint",
        slint_build::CompilerConfiguration::new()
            .with_style("fluent-light".to_owned())
            .embed_resources(slint_build::EmbedResourcesKind::EmbedForSoftwareRenderer)
            .with_sdf_fonts(false) // For this smallest font size, we do off. Above it, everything is true
    )
    .unwrap();
}
