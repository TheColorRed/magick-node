{
    'targets': [
        {
            'target_name': 'NodeImageMagick',
            'sources': [
                'src/addon.cc',
                'src/imageMagick.cc'
            ],
            'include_dirs': [
                'ImageMagick/include'
            ],
            'link_settings': {
                'libraries': [
                    '<(module_root_dir)/ImageMagick/lib/CORE_RL_Magick++_.lib',
                    '<(module_root_dir)/ImageMagick/lib/CORE_RL_MagickCore_.lib',
                    '<(module_root_dir)/ImageMagick/lib/CORE_RL_MagickWand_.lib',
                ]
            }
        }
    ]
}
