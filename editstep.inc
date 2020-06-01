<?php
// $Id$
/******************************************************************************/
/***************************** EDIT EXAMPLE ***********************************/
/******************************************************************************/
function upload_step_edit_form($form, &$form_state, $no_js_use = FALSE)
  {
    global $user;
    $step_id = arg(4);
    /* get example details */
    /*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $example_id);
    $example_data = db_fetch_object($example_q);*/
    $query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('id',$step_id);
    $step_q = $query->execute();
    $step_data = $step_q->fetchObject();
    //var_dump($step_data);die;
    $query = db_select('arduino_projects_blog_step_information');
    $query->fields('arduino_projects_blog_step_information');
    $query->condition('step_id', $step_id);
    $query->range(0, 1);
    $step_info_q = $query->execute();
    $step_info_data = $step_info_q->fetchObject();
    if (!$step_info_data)
      {
        drupal_set_message(t("Invalid step selected."), 'error');
        drupal_goto('');
        return;
      }
    if ($step_info_data->approval_status != 0)
      {
        drupal_set_message(t("You cannot edit an example after it has been approved or dis-approved. Please contact site administrator if you want to edit this example."), 'error');
        drupal_goto('');
        return;
      }
      //var_dump($step_info_data);die;
    /* get examples files */
    $images = "";
    $images_id = 0;
    $gifs = "";
    $gifs_id = 0;
    /*$step_files_q = db_query("SELECT * FROM {arduino_projects_blog_step_files} WHERE example_id = %d", $example_id);*/
    $query = db_select('arduino_projects_blog_step_files');
    $query->fields('arduino_projects_blog_step_files');
    $query->condition('step_id', $step_id);
    $step_files_q = $query->execute();
    while ($step_files_data = $step_files_q->fetchObject())
      {
        if ($step_files_data->filetype == "I")
          {
            $images = l($step_files_data->filename, 'arduino-projects/download/file/' . $step_files_data->id);
            $images_id = $step_files_data->id;
            //var_dump($source_file);die;
          }
        else if ($step_files_data->filetype == "G")
          {
            $gifs = l($step_files_data->filename, 'arduino-projects/download/file/' . $step_files_data->id);
            $gifs_id = $step_files_data->id;
            //var_dump($source_file);die;
          }
      }
    /* get chapter details */
    /*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d", $example_data->chapter_id);
    $chapter_data = db_fetch_object($chapter_q);*/
    // $query = db_select('textbook_companion_chapter');
    // $query->fields('textbook_companion_chapter');
    // $query->condition('id', $example_data->chapter_id);
    // $result = $query->execute();
    // $chapter_data = $result->fetchObject();
    // if (!$chapter_data)
    //   {
    //     drupal_set_message(t("Invalid chapter selected."), 'error');
    //     drupal_goto('');
    //     return;
    //   }
    /* get preference details */
    /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $chapter_data->preference_id);
    $preference_data = db_fetch_object($preference_q);*/
    /*$query = db_select('textbook_companion_preference');
    $query->fields('textbook_companion_preference');
    $query->condition('id', $chapter_data->preference_id);
    $result = $query->execute();
    $preference_data = $result->fetchObject();
    if (!$preference_data)
      {
        drupal_set_message(t("Invalid book selected."), 'error');
        drupal_goto('');
        return;
      }
    if ($preference_data->approval_status != 1)
      {
        drupal_set_message(t("Cannot edit example. Either the book proposal has not been approved or it has been rejected."), 'error');
        drupal_goto('');
        return;
      }*/
    /* get proposal details */
    /*$proposal_q = db_query("SELECT * FROM {arduino_projects_blog_proposal} WHERE id = %d", $preference_data->proposal_id);
    $proposal_data = db_fetch_object($proposal_q);*/
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('id', $step_data->proposal_id);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message(t("Invalid proposal selected."), 'error');
        drupal_goto('');
        return;
      }
    if ($proposal_data->uid != $user->uid)
      {
        drupal_set_message(t("You do not have permissions to edit this example."), 'error');
        drupal_goto('');
        return;
      }
    $user_data = user_load($proposal_data->uid);
    $form['#redirect'] = 'arduino-projects/code';
    $form['#attributes'] = array(
        'enctype' => "multipart/form-data"
    );
    $form['project_title'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data->project_title,
        '#title' => t('Project title')
    );
    $form['contributor_name'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data->contributor_name,
        '#title' => t('Contributor Name')
    );
    $form['number'] = array(
        '#type' => 'item',
        '#title' => t('Step No'),
        '#markup' => $step_data->number
    );
    if($step_data->number <= 5){
    $form['name'] = array(
        '#type' => 'item',
        '#title' => t('Title of the Step'),
        '#markup' => $step_data->name
    );
    }
    else{
        $form['name'] = array(
        '#type' => 'textfield',
        '#title' => t('Title of the Step'),
        '#default_value' => $step_data->name
    );
    }
    $form['step_description'] = array(
        '#type' => 'text_format',
        '#title' => t('Description'),
        '#required' => TRUE,
        '#default_value' => $step_info_data->description,
        '#format' => $step_info_data->description_type
    );
    $form['video'] = array(
        '#type' => 'textarea',
        '#title' => t('Video'),
        '#default_value' => $step_info_data->video
    );
    //$dest_path = $proposal_data->directory_name . '/'
    if ($images)
      {
        $form['uplpoaded_images'] = array(
            '#markup' => '<h5><p> Edit existing step images</p></h5>(Select Check box to delete existing photos)',
            '#prefix' => '<div id="uploaded_images"><table><tr>',
            '#suffix' => ''
        );
        $query = db_select('arduino_projects_blog_step_files');
        $query->fields('arduino_projects_blog_step_files');
        $query->condition('step_id', $step_id);
        $query->condition('filetype', 'I');
        $step_files_q = $query->execute();
        /*$query1 = db_select('arduino_projects_blog_proposal_images');
        $query1->fields('arduino_projects_blog_proposal_images');
        $query1->condition('proposal_id', $proposal_id);
        $result1 = $query1->execute();*/
        while ($row1 = $step_files_q->fetchObject()) {
            $form['imagecheck@' . $row1->id] = array(
                '#type' => 'checkbox',
                '#field_suffix' => '<img style="width:500px; padding-left :20px;" src=' . $GLOBALS['base_url'] . "/project_uploads/" . $row1->filepath . ' />'
            );
        }
        $form['enduploadphotos'] = array(
            '#markup' => '',
            '#prefix' => '',
            '#suffix' => '</tr></table></div>'
        );
        $form['step_images'] = array(
            '#type' => 'fieldset',
            '#tree' => TRUE,
            '#prefix' => '<div id="names-fieldset-wrapper">',
            '#suffix' => '</div>',
            '#title' => t('Upload images related to the step'),
        );
        if (empty($form_state['image_names'])) {
            $form_state['image_names'] = 1;
        }
        for ($i = 0; $i < $form_state['image_names']; $i++) {
            $temp_no = $i;
            $form['step_images'][$i]['image'] = array(
                //'#title' => t('Add Event Image'),
                '#type' => 'file',
                '#weight' => '5',
                '#description' => t('Upload filenames with allowed extensions only. No spaces or any special characters allowed in filename.') . '<br />' . t('<span style="color:red;">Allowed file extensions: ') . variable_get('step_image_extensions', '') . t('</span>'),
                // We need this to know which file element this is.
                // By default drupal would name all as files[step_images]
                '#name' => 'files[step_images_' . $i . '_name]',
                //'#attributes' => array('multiple' => 'multiple'),
                //'#upload_location' => 'public://',
                '#upload_validators' => array(
                    // Pass the maximum file size in bytes
                    'file_validate_size' => array(2*1024),
                  ),
            );
            $form['step_images']["images_count"] = array(
                "#type" => "hidden",
                "#value" => $temp_no
            );
        }
        if($i < 5){
        $form['step_images']['add_image'] = array(
            '#type' => 'submit',
            '#value' => t('Add Image'),
            '#limit_validation_errors' => array(),
            '#submit' => array(
                'step_images_add_more_add_one'
            ),
            '#ajax' => array(
                'callback' => 'step_images_add_more_callback',
                'wrapper' => 'names-fieldset-wrapper'
            )
        );
    }
        if ($form_state['image_names'] > 1) {
            $form['step_images']['remove_image'] = array(
                '#type' => 'submit',
                '#value' => t('Remove Image'),
                '#limit_validation_errors' => array(),
                '#submit' => array(
                    'step_images_add_more_remove_one'
                ),
                '#ajax' => array(
                    'callback' => 'step_images_add_more_callback',
                    'wrapper' => 'names-fieldset-wrapper'
                )
            );
        }
        if ($no_js_use) {
            if (!empty($form['step_images']['remove_image']['#ajax'])) {
                unset($form['step_images']['remove_image']['#ajax']);
            }
            unset($form['step_images']['add_image']['#ajax']);
        }
        
      }
    else
      {
        $form['step_images'] = array(
            '#type' => 'fieldset',
            '#tree' => TRUE,
            '#prefix' => '<div id="names-fieldset-wrapper">',
            '#suffix' => '</div>',
            '#title' => t('Upload images related to the step'),
        );
        if (empty($form_state['image_names'])) {
            $form_state['image_names'] = 1;
        }
        for ($i = 0; $i < $form_state['image_names']; $i++) {
            $temp_no = $i;
            $form['step_images'][$i]['image'] = array(
                //'#title' => t('Add Event Image'),
                '#type' => 'file',
                '#weight' => '5',
                '#description' => t('Upload filenames with allowed extensions only. No spaces or any special characters allowed in filename.') . '<br />' . t('<span style="color:red;">Allowed file extensions: ') . variable_get('step_image_extensions', '') . t('</span>'),
                // We need this to know which file element this is.
                // By default drupal would name all as files[step_images]
                '#name' => 'files[step_images_' . $i . '_name]',
                //'#attributes' => array('multiple' => 'multiple'),
                //'#upload_location' => 'public://',
                '#upload_validators' => array(
                    // Pass the maximum file size in bytes
                    'file_validate_size' => array(2*1024),
                  ),
            );
            $form['step_images']["images_count"] = array(
                "#type" => "hidden",
                "#value" => $temp_no
            );
        }
        if($i < 5){
        $form['step_images']['add_image'] = array(
            '#type' => 'submit',
            '#value' => t('Add Image'),
            '#limit_validation_errors' => array(),
            '#submit' => array(
                'step_images_add_more_add_one'
            ),
            '#ajax' => array(
                'callback' => 'step_images_add_more_callback',
                'wrapper' => 'names-fieldset-wrapper'
            )
        );
    }
        if ($form_state['image_names'] > 1) {
            $form['step_images']['remove_image'] = array(
                '#type' => 'submit',
                '#value' => t('Remove Image'),
                '#limit_validation_errors' => array(),
                '#submit' => array(
                    'step_images_add_more_remove_one'
                ),
                '#ajax' => array(
                    'callback' => 'step_images_add_more_callback',
                    'wrapper' => 'names-fieldset-wrapper'
                )
            );
        }
        if ($no_js_use) {
            if (!empty($form['step_images']['remove_image']['#ajax'])) {
                unset($form['step_images']['remove_image']['#ajax']);
            }
            unset($form['step_images']['add_image']['#ajax']);
        }
      }
    if ($gifs)
      {
        $form['uplpoaded_gifs'] = array(
            '#markup' => '<h5><p> Edit existing step gifs</p></h5>(Select Check box to delete existing photos)',
            '#prefix' => '<div id="uploaded_gifs"><table><tr>',
            '#suffix' => ''
        );
        $query = db_select('arduino_projects_blog_step_files');
        $query->fields('arduino_projects_blog_step_files');
        $query->condition('step_id', $step_id);
        $query->condition('filetype', 'G');
        $step_files_q = $query->execute();
        /*$query1 = db_select('arduino_projects_blog_proposal_gifs');
        $query1->fields('arduino_projects_blog_proposal_gifs');
        $query1->condition('proposal_id', $proposal_id);
        $result1 = $query1->execute();*/
        while ($row1 = $step_files_q->fetchObject()) {
            $form['gifcheck@' . $row1->id] = array(
                '#type' => 'checkbox',
                '#field_suffix' => '<img style="width:500px; padding-left :20px;" src=' . $GLOBALS['base_url'] . "/project_uploads/" . $row1->filepath . ' />'
            );
        }
        $form['enduploadphotos'] = array(
            '#markup' => '',
            '#prefix' => '',
            '#suffix' => '</tr></table></div>'
        );
        $form['step_gifs'] = array(
            '#type' => 'fieldset',
            '#tree' => TRUE,
            '#prefix' => '<div id="gifs-fieldset-wrapper">',
            '#suffix' => '</div>',
            '#title' => t('Upload gifs related to the step'),
        );
        if (empty($form_state['gif_names'])) {
            $form_state['gif_names'] = 1;
        }
        for ($p = 0; $p < $form_state['gif_names']; $p++) {
            $temp_no = $p;
            $form['step_gifs'][$p]['gifs'] = array(
                //'#title' => t('Add Event Image'),
                '#type' => 'file',
                '#weight' => '5',
                '#description' => t('Upload filenames with allowed extensions only. No spaces or any special characters allowed in filename.') . '<br />' . t('<span style="color:red;">Allowed file extensions: gif</span>'),
                // We need this to know which file element this is.
                // By default drupal would name all as files[step_gifs]
                '#name' => 'files[step_gifs_' . $p . '_name]',
                //'#attributes' => array('multiple' => 'multiple'),
                //'#upload_location' => 'public://',
                '#upload_validators' => array(
                    'file_validate_extensions' => ['gif'],
                    // Pass the maximum file size in bytes
                    'file_validate_size' => array(2*1024),
                  ),
            );
            $form['step_gifs']["images_count"] = array(
                "#type" => "hidden",
                "#value" => $temp_no
            );
        }
        if($p < 5){
        $form['step_gifs']['add_gif'] = array(
            '#type' => 'submit',
            '#value' => t('Add Gif'),
            '#limit_validation_errors' => array(),
            '#submit' => array(
                'step_gifs_add_more_add_one'
            ),
            '#ajax' => array(
                'callback' => 'step_gifs_add_more_callback',
                'wrapper' => 'gifs-fieldset-wrapper'
            )
        );
    }
        if ($form_state['gif_names'] > 1) {
            $form['step_gifs']['remove_gif'] = array(
                '#type' => 'submit',
                '#value' => t('Remove Gif'),
                '#limit_validation_errors' => array(),
                '#submit' => array(
                    'step_gifs_add_more_remove_one'
                ),
                '#ajax' => array(
                    'callback' => 'step_gifs_add_more_callback',
                    'wrapper' => 'gifs-fieldset-wrapper'
                )
            );
        }
        if ($no_js_use) {
            if (!empty($form['step_gifs']['remove_gif']['#ajax'])) {
                unset($form['step_gifs']['remove_gif']['#ajax']);
            }
            unset($form['step_gifs']['add_gif']['#ajax']);
        }
        
      }
    else
      {
        $form['step_gifs'] = array(
            '#type' => 'fieldset',
            '#tree' => TRUE,
            '#prefix' => '<div id="gifs-fieldset-wrapper">',
            '#suffix' => '</div>',
            '#title' => t('Upload gifs related to the step'),
        );
        if (empty($form_state['gif_names'])) {
            $form_state['gif_names'] = 1;
        }
        for ($p = 0; $p < $form_state['gif_names']; $p++) {
            $temp_no = $p;
            $form['step_gifs'][$p]['gifs'] = array(
                //'#title' => t('Add Event Image'),
                '#type' => 'file',
                '#weight' => '5',
                '#description' => t('Upload filenames with allowed extensions only. No spaces or any special characters allowed in filename.') . '<br />' . t('<span style="color:red;">Allowed file extensions: gif</span>'),
                // We need this to know which file element this is.
                // By default drupal would name all as files[step_gifs]
                '#name' => 'files[step_gifs_' . $p . '_name]',
                //'#attributes' => array('multiple' => 'multiple'),
                //'#upload_location' => 'public://',
                '#upload_validators' => array(
                    'file_validate_extensions' => ['gif'],
                    // Pass the maximum file size in bytes
                    'file_validate_size' => array(2*1024),
                  ),
            );
            $form['step_gifs']["images_count"] = array(
                "#type" => "hidden",
                "#value" => $temp_no
            );
        }
        if($p < 5){
        $form['step_gifs']['add_gif'] = array(
            '#type' => 'submit',
            '#value' => t('Add Gif'),
            '#limit_validation_errors' => array(),
            '#submit' => array(
                'step_gifs_add_more_add_one'
            ),
            '#ajax' => array(
                'callback' => 'step_gifs_add_more_callback',
                'wrapper' => 'gifs-fieldset-wrapper'
            )
        );
    }
        if ($form_state['gif_names'] > 1) {
            $form['step_gifs']['remove_gif'] = array(
                '#type' => 'submit',
                '#value' => t('Remove Gif'),
                '#limit_validation_errors' => array(),
                '#submit' => array(
                    'step_gifs_add_more_remove_one'
                ),
                '#ajax' => array(
                    'callback' => 'step_gifs_add_more_callback',
                    'wrapper' => 'gifs-fieldset-wrapper'
                )
            );
        }
        if ($no_js_use) {
            if (!empty($form['step_gifs']['remove_gif']['#ajax'])) {
                unset($form['step_gifs']['remove_gif']['#ajax']);
            }
            unset($form['step_gifs']['add_gif']['#ajax']);
        }
      }
    
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit')
    );
    $form['cancel'] = array(
        '#type' => 'item',
        '#markup' => l(t('Cancel'), 'arduino-projects/code')
    );
    return $form;
  }
function step_images_add_more_callback($form, $form_state) {
    return $form['step_images'];
}


function step_images_add_more_add_one($form, &$form_state) {
    $form_state['image_names']++;
    $form_state['rebuild'] = TRUE;
    //$form_state['no_redirect'] = TRUE;
}


function step_images_add_more_remove_one($form, &$form_state) {
    if ($form_state['image_names'] > 1) {
        $form_state['image_names']--;
    }
    $form_state['rebuild'] = TRUE;
}
function step_gifs_add_more_callback($form, $form_state) {
    return $form['step_gifs'];
}


function step_gifs_add_more_add_one($form, &$form_state) {
    $form_state['gif_names']++;
    $form_state['rebuild'] = TRUE;
    //$form_state['no_redirect'] = TRUE;
}


function step_gifs_add_more_remove_one($form, &$form_state) {
    if ($form_state['gif_names'] > 1) {
        $form_state['gif_names']--;
    }
    $form_state['rebuild'] = TRUE;
}
function upload_step_edit_form_validate($form, &$form_state)
  {
   /*if (isset($_FILES['files']))
      {
        /* check for valid filename extensions 
        foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
          {
            if ($file_name)
              {
                if (strstr($file_form_name, 'image'))
                        $file_type = 'I';
                else if (strstr($file_form_name, 'gifs'))
                        $file_type = 'G';
                $allowed_extensions_str = '';
                switch ($file_type)
                    {
                        case 'I':
                            $allowed_extensions_str = variable_get('step_image_extensions', '');
                            break;
                        case 'G':
                            $allowed_extensions_str = 'gif';
                            break;
                    }
                //$allowed_extensions_str = variable_get('arduino_projects_blog_source_extensions', '');
                $allowed_extensions = explode(',', $allowed_extensions_str);
                $temp_ext = explode('.', strtolower($_FILES['files']['name'][$file_form_name]));
                $temp_extension = end($temp_ext);
                $temp_extension = substr($_FILES['files']['name'][$file_form_name], strripos($_FILES['files']['name'][$file_form_name], '.')); // get file name
                //var_dump($temp_extension); die;
                if (!in_array($temp_extension, $allowed_extensions))
                    form_set_error($file_form_name, t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
                if ($_FILES['files']['size'][$file_form_name] <= 0)
                    form_set_error($file_form_name, t('File size cannot be zero.'));
                /* check if valid file name 
                if (!arduino_projects_blog_check_valid_filename($_FILES['files']['name'][$file_form_name]))
                form_set_error($file_form_name, t('Invalid file name specified. Only alphabets, numbers and underscore is allowed as a valid filename.'));
              }
          }
      }*/
  }
function upload_step_edit_form_submit($form, &$form_state)
  {
    global $user;
    $step_id = arg(4);
    /* get example details */
    /*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $example_id);
    $example_data = db_fetch_object($example_q);*/
    $query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('id',$step_id);
    $step_q = $query->execute();
    $step_data = $step_q->fetchObject();
    //var_dump($step_data);die;
    $query = db_select('arduino_projects_blog_step_information');
    $query->fields('arduino_projects_blog_step_information');
    $query->condition('step_id', $step_id);
    $query->range(0, 1);
    $step_info_q = $query->execute();
    $step_info_data = $step_info_q->fetchObject();
    if (!$step_info_data)
      {
        drupal_set_message(t("Invalid step selected."), 'error');
        drupal_goto('');
        return;
      }
    if ($step_info_data->approval_status != 0)
      {
        drupal_set_message(t("You cannot edit an example after it has been approved or dis-approved. Please contact site administrator if you want to edit this example."), 'error');
        drupal_goto('');
        return;
      }
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('id', $step_data->proposal_id);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message(t("Invalid proposal selected."), 'error');
        drupal_goto('');
        return;
      }
    if ($proposal_data->uid != $user->uid)
      {
        drupal_set_message(t("You do not have permissions to edit this example."), 'error');
        drupal_goto('');
        return;
      }
    /* creating directories */
    $root_path = arduino_projects_blog_files_path();
    $dest_path = $proposal_data->directory_name . '/';
    if (!is_dir($root_path . $dest_path))
        mkdir($root_path . $dest_path);
    $dest_path .= 'project_files/';
    if (!is_dir($root_path . $dest_path))
        mkdir($root_path . $dest_path);
    $dest_path .= 'Step' . $step_data->number . '/';
    if (!is_dir($root_path . $dest_path))
        mkdir($root_path . $dest_path);
    $filepath = $proposal_data->directory_name . '/project_files/Step' . $step_data->number . '/';
    /* updating example caption */
    /*db_query("UPDATE {textbook_companion_example} SET caption = '%s' WHERE id = %d", $form_state['values']['example_caption'], $example_id);*/
    if($step_data->number > 5){
    $query = db_update('arduino_projects_blog_step');
    $query->fields(array(
        'name' => $form_state['values']['name']
    ));
    $query->condition('id', $step_id);
    $num_updated = $query->execute();
    }
    $query = db_update('arduino_projects_blog_step_information');
    $query->fields(array(
        'description' => $form_state['values']['step_description']['value'],
        'description_type' => $form_state['values']['step_description']['format'],
        'video' => $form_state['values']['video'],
        'timestamp' => time()
    ));
    $query->condition('step_id', $step_id);
    $step_info_updated = $query->execute();
    $deletecounter_images = 0;
    $query_img = db_select('arduino_projects_blog_step_files');
    $query_img->fields('arduino_projects_blog_step_files');
    $query_img->condition('step_id', $step_id);
    $query_img->condition('filetype', 'I');
    $result_img = $query_img->execute();
    //$root_path= arduino_projects_blog_files_path();
        while ($row_img = $result_img->fetchObject()) {
            if ($form_state['values']['imagecheck@' . $row_img->id] == 1) {
                if (file_exists($root_path . $row_img->filepath)) {
                    unlink($root_path .$row_img->filepath);
                    $query2 = db_delete('arduino_projects_blog_step_files');
                    $query2->condition('id', $row_img->id);
                    $delete_img = $query2->execute();
                    if ($delete_img != 0) {
                        $deletecounter_images++;
                    }
                } 
                else {
                    drupal_set_message(t('Error Could not delete :') . $filename . t(', file does not exist'), 'error');
                }
            }
        }
    $deletecounter_gifs = 0;
    $query_img = db_select('arduino_projects_blog_step_files');
    $query_img->fields('arduino_projects_blog_step_files');
    $query_img->condition('step_id', $step_id);
    $query_img->condition('filetype', 'G');
    $result_img = $query_img->execute();
    //$root_path= arduino_projects_blog_files_path();
        while ($row_img = $result_img->fetchObject()) {
            if ($form_state['values']['gifcheck@' . $row_img->id] == 1) {
                if (file_exists($root_path . $row_img->filepath)) {
                    unlink($root_path .$row_img->filepath);
                    $query2 = db_delete('arduino_projects_blog_step_files');
                    $query2->condition('id', $row_img->id);
                    $delete_img = $query2->execute();
                    if ($delete_img != 0) {
                        $deletecounter_gifs++;
                    }
                } 
                else {
                    drupal_set_message(t('Error Could not delete :') . $filename . t(', file does not exist'), 'error');
                }
            }
        }
    foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
      {
        if ($file_name)
          {
            /* checking file type */
            if (strstr($file_form_name, 'images'))
            {
                $file_type = 'I';
            } //strstr($file_form_name, 'upload_flowsheet_developed_process')
            else if (strstr($file_form_name, 'gifs'))
            {
                $file_type = 'G';
            }

            //$file_type = 'S';
            switch ($file_type) {
                case 'I':
                    $dest_path = $filepath . 'images/';
                    //var_dump($root_path . $dest_path);die;
                    if (!is_dir($root_path . $dest_path))
                        mkdir($root_path . $dest_path);
                    if (file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
                      {
                        drupal_set_message(t("Error uploading file. File !filename already exists.", array(
                            '!filename' => $_FILES['files']['name'][$file_form_name]
                        )), 'error');
                        return;
                      }
                    /* uploading file */
                    else if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
                      {
                        $query = "INSERT INTO {arduino_projects_blog_step_files} (step_id, filename, filepath,filemime, filesize, filetype, timestamp)
                  VALUES (:step_id, :filename ,:filepath,:filemime, :filesize, :filetype, :timestamp)";
                        $args = array(
                            ":step_id" => $step_id,
                            ":filename" => $_FILES['files']['name'][$file_form_name],
                            ":filepath" => $dest_path . $_FILES['files']['name'][$file_form_name],
                            ":filemime" => mime_content_type($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]),
                            ":filesize" => $_FILES['files']['size'][$file_form_name],
                            ":filetype" => $file_type,
                            ":timestamp" => time()
                        );
                        $result = db_query($query, $args, array(
                            'return' => Database::RETURN_INSERT_ID
                        ));
                        drupal_set_message($file_name . ' uploaded successfully.', 'status');
                      }
                    else
                      {
                        drupal_set_message('Error uploading file : ' . $dest_path . $file_name, 'error');
                      }
                      break;
                case 'G':
                    $dest_path = $filepath . 'gifs/';
                    //var_dump($root_path . $dest_path);die;
                    if (!is_dir($root_path . $dest_path))
                        mkdir($root_path . $dest_path);
                    //var_dump($root_path . $dest_path);die;
                    if (file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
                      {
                        drupal_set_message(t("Error uploading file. File !filename already exists.", array(
                            '!filename' => $_FILES['files']['name'][$file_form_name]
                        )), 'error');
                        return;
                      }
                    /* uploading file */
                    else if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
                      {
                        $query = "INSERT INTO {arduino_projects_blog_step_files} (step_id, filename, filepath,filemime, filesize, filetype, timestamp)
                  VALUES (:step_id, :filename ,:filepath,:filemime, :filesize, :filetype, :timestamp)";
                        $args = array(
                            ":step_id" => $step_id,
                            ":filename" => $_FILES['files']['name'][$file_form_name],
                            ":filepath" => $dest_path . $_FILES['files']['name'][$file_form_name],
                            ":filemime" => mime_content_type($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]),
                            ":filesize" => $_FILES['files']['size'][$file_form_name],
                            ":filetype" => $file_type,
                            ":timestamp" => time()
                        );
                        $result = db_query($query, $args, array(
                            'return' => Database::RETURN_INSERT_ID
                        ));
                        drupal_set_message($file_name . ' uploaded successfully.', 'status');
                      }
                    else
                      {
                        drupal_set_message('Error uploading file : ' . $dest_path . '/' . $file_name, 'error');
                      }
                      break;
            }
          }
      }
    drupal_set_message('Step uploaded successfully.', 'status');
    /* sending email */
    $email_to = $user->mail;
    $from = variable_get('textbook_companion_from_email', '');
    $bcc = variable_get('textbook_companion_emails', '');
    $cc = variable_get('textbook_companion_cc_emails', '');
    $param['example_updated']['step_id'] = $step_id;
    $param['example_updated']['user_id'] = $user->uid;
    $param['example_updated']['headers'] = array(
        'From' => $from,
        'MIME-Version' => '1.0',
        'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
        'Content-Transfer-Encoding' => '8Bit',
        'X-Mailer' => 'Drupal',
        'Cc' => $cc,
        'Bcc' => $bcc
    );
    /*if (!drupal_mail('textbook_companion', 'example_updated', $email_to, language_default(), $param, $from, TRUE))
        drupal_set_message('Error sending email message.', 'error');
    drupal_set_message(t("Step successfully udpated."), 'status');*/
    drupal_goto('arduino-projects/code');
  }
/******************************************************************************/
/**************************** EDIT CHAPTER TITLE ******************************/
/******************************************************************************/
function edit_chapter_title_form($form, $form_state)
  {
    global $user;
    /************************ start approve book details ************************/
    /*$proposal_q = db_query("SELECT * FROM {arduino_projects_blog_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
    $proposal_data = db_fetch_object($proposal_q);*/
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('uid', $user->uid);
    $query->orderBy('id', 'DESC');
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message("Please submit a " . l('proposal', 'arduino-projects/proposal') . ".", 'error');
        drupal_goto('arduino-projects/code');
      }
    if ($proposal_data->proposal_status != 1 && $proposal_data->proposal_status != 4)
      {
        switch ($proposal_data->proposal_status)
        {
            case 0:
                drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
                drupal_goto('arduino-projects/code');
                return;
                break;
            case 2:
                drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'proposal') . '.'), 'error');
                drupal_goto('arduino-projects/code');
                return;
                break;
            case 3:
                drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'arduino-projects/proposal') . '.'), 'status');
                drupal_goto('arduino-projects/code');
                return;
                break;
            default:
                drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
                drupal_goto('arduino-projects/code');
                return;
                break;
        }
      }
    /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
    $preference_data = db_fetch_object($preference_q);*/
    $query = db_select('textbook_companion_preference');
    $query->fields('textbook_companion_preference');
    $query->condition('proposal_id', $proposal_data->id);
    $query->condition('approval_status', 1);
    $query->range(0, 1);
    $result = $query->execute();
    $preference_data = $result->fetchObject();
    if (!$preference_data)
      {
        drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
        drupal_goto('arduino-projects/code');
        return;
      }
    /************************ end approve book details **************************/
    $chapter_id = arg(4);
    /*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d AND preference_id = %d", $chapter_id, $preference_data->id);
    $chapter_data = db_fetch_object($chapter_q);*/
    $query = db_select('textbook_companion_chapter');
    $query->fields('textbook_companion_chapter');
    $query->condition('id', $chapter_id);
    $query->condition('preference_id', $preference_data->id);
    $result = $query->execute();
    $chapter_data = $result->fetchObject();
    if (!$chapter_data)
      {
        drupal_set_message(t('Invalid chapter.'), 'error');
        drupal_goto('arduino-projects/code');
        return;
      }
    $form['#redirect'] = 'arduino-projects/code';
    $form['book_details']['book'] = array(
        '#type' => 'item',
        '#markup' => $preference_data->book,
        '#title' => t('Title of the Book')
    );
    $form['contributor_name'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data->full_name,
        '#title' => t('Contributor Name')
    );
    $form['number'] = array(
        '#type' => 'item',
        '#title' => t('Chapter No'),
        '#markup' => $chapter_data->number
    );
    $form['chapter_title'] = array(
        '#type' => 'textfield',
        '#title' => t('Title of the Chapter'),
        '#size' => 40,
        '#maxlength' => 255,
        '#required' => TRUE,
        '#default_value' => $chapter_data->name
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit')
    );
    $form['cancel'] = array(
        '#type' => 'markup',
        '#value' => l(t('Cancel'), 'textbook_companion/code')
    );
    return $form;
  }
function edit_chapter_title_form_validate($form, &$form_state)
  {
    if (!check_name($form_state['values']['chapter_title']))
        form_set_error('chapter_title', t('Title of the Chapter can contain only alphabets, numbers and spaces.'));
  }
function edit_chapter_title_form_submit($form, &$form_state)
  {
    global $user;
    /************************ start approve book details ************************/
    /*$proposal_q = db_query("SELECT * FROM {arduino_projects_blog_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
    $proposal_data = db_fetch_object($proposal_q);*/
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('uid', $user->uid);
    $query->orderBy('id', 'DESC');
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message("Please submit a " . l('proposal', 'arduino-projects/proposal') . ".", 'error');
        drupal_goto('arduino-projects/code');
      }
    if ($proposal_data->proposal_status != 1 && $proposal_data->proposal_status != 4)
      {
        switch ($proposal_data->proposal_status)
        {
            case 0:
                drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
                drupal_goto('arduino-projects/code');
                return;
                break;
            case 2:
                drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'proposal') . '.'), 'error');
                drupal_goto('arduino-projects/code');
                return;
                break;
            case 3:
                drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'arduino-projects/proposal') . '.'), 'status');
                drupal_goto('arduino-projects/code');
                return;
                break;
            default:
                drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
                drupal_goto('arduino-projects/code');
                return;
                break;
        }
      }
    /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
    $preference_data = db_fetch_object($preference_q);*/
    $query = db_select('textbook_companion_preference');
    $query->fields('textbook_companion_preference');
    $query->condition('proposal_id', $proposal_data->id);
    $query->condition('approval_status', 1);
    $query->range(0, 1);
    $result = $query->execute();
    $preference_data = $result->fetchObject();
    if (!$preference_data)
      {
        drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
        drupal_goto('arduino-projects/code');
        return;
      }
    /************************ end approve book details **************************/
    $chapter_id = arg(4);
    /*$chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d AND preference_id = %d", $chapter_id, $preference_data->id);
    $chapter_data = db_fetch_object($chapter_q);*/
    $query = db_select('textbook_companion_chapter');
    $query->fields('textbook_companion_chapter');
    $query->condition('id', $chapter_id);
    $query->condition('preference_id', $preference_data->id);
    $result = $query->execute();
    $chapter_data = $result->fetchObject();
    if (!$chapter_data)
      {
        drupal_set_message(t('Invalid chapter.'), 'error');
        drupal_goto('textbookcompanion/code');
        return;
      }
    /*db_query("UPDATE {textbook_companion_chapter} SET name = '%s' WHERE id = %d", $form_state['values']['chapter_title'], $chapter_id);*/
    $query = db_update('textbook_companion_chapter');
    $query->fields(array(
        'name' => $form_state['values']['chapter_title']
    ));
    $query->condition('id', $chapter_id);
    $num_updated = $query->execute();
    drupal_set_message(t('Title of the Chapter updated.'), 'status');
  }
/******************************************************************************/
/************************** GENERAL FUNCTIONS *********************************/
/******************************************************************************/
function _list_of_book_titles()
  {
    $book_titles = array(
        '0' => 'Please select...'
    );
    /*$book_titles_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE approval_status = 1 OR approval_status = 3 ORDER BY book ASC");*/
    $query = db_select('textbook_companion_preference');
    $query->fields('textbook_companion_preference');
    $or = db_or();
    $or->condition('approval_status', 1);
    $or->condition('approval_status', 3);
    $query->condition($or);
    $query->orderBy('book', 'ASC');
    $book_titles_q = $query->execute();
    while ($book_titles_data = $book_titles_q->fetchObject())
      {
        $book_titles[$book_titles_data->id] = $book_titles_data->book . ' (Written by ' . $book_titles_data->author . ')';
      }
    return $book_titles;
  }
