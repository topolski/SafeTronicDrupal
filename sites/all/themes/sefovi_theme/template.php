<?php
/**
 * Implements hook_html_head_alter().
 * This will overwrite the default meta character type tag with HTML5 version.
 */
function sefovi_theme_html_head_alter(&$head_elements) {
  $head_elements['system_meta_content_type']['#attributes'] = array(
    'charset' => 'utf-8'
  );
}

/**
 * Insert themed breadcrumb page navigation at top of the node content.
 */
function sefovi_theme_breadcrumb($variables) {
  $breadcrumb = $variables['breadcrumb'];
  if (!empty($breadcrumb)) {
    // Use CSS to hide titile .element-invisible.
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';
    // comment below line to hide current page to breadcrumb
	$breadcrumb[] = drupal_get_title();
  $breadcrumb[0] = l('Naslovna',NULL);
    $output .= '<nav class="breadcrumb">' . implode(' » ', $breadcrumb) . '</nav>';
    return $output;
  }
}

/**
 * Override or insert variables into the page template.
 */
function sefovi_theme_preprocess_page(&$vars) {
  if (isset($vars['main_menu_links'])) {
    $vars['main_menu'] = theme('links__system_main_menu', array(
      'links' => $vars['main_menu_links'],
      'attributes' => array(
        'class' => array('links', 'main-menu', 'clearfix'),
      ),
      'heading' => array(
        'text' => t('Main menu'),
        'level' => 'h2',
        'class' => array('element-invisible'),
      )
    ));
  }
  if (isset($vars['secondary_menu_links'])) {
    $vars['secondary_menu'] = theme('links__system_secondary_menu', array(
      'links' => $vars['secondary_menu_links'],
      'attributes' => array(
        'class' => array('links', 'secondary-menu', 'clearfix'),
      ),
      'heading' => array(
        'text' => t('Secondary menu'),
        'level' => 'h2',
        'class' => array('element-invisible'),
      )
    ));
  }
  // Add javascript files for front-page jquery slideshow.
  if (drupal_is_front_page() || theme_get_setting('slideshow_all')) {
    $directory = drupal_get_path('theme', 'sefovi_theme');
    drupal_add_js($directory . '/js/jquery.flexslider-min.js', array('group' => JS_THEME));
    drupal_add_js($directory . '/js/slide.js', array('group' => JS_THEME));
  }
  // <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1"/>
  $viewport = array(
    '#type' => 'html_tag',
    '#tag' => 'meta',
    '#attributes' => array(
      'name' =>  'viewport',
      'content' =>  'width=device-width'
    )
  );
  drupal_add_html_head($viewport, 'viewport');
}

/**
 * Duplicate of theme_menu_local_tasks() but adds clearfix to tabs.
 */
function sefovi_theme_menu_local_tasks(&$variables) {
  $output = '';

  if (!empty($variables['primary'])) {
    $variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $variables['primary']['#prefix'] .= '<ul class="tabs primary clearfix">';
    $variables['primary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['primary']);
  }
  if (!empty($variables['secondary'])) {
    $variables['secondary']['#prefix'] = '<h2 class="element-invisible">' . t('Secondary tabs') . '</h2>';
    $variables['secondary']['#prefix'] .= '<ul class="tabs secondary clearfix">';
    $variables['secondary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['secondary']);
  }
  return $output;
}

/**
 * Override or insert variables into the node template.
 */
function sefovi_theme_preprocess_node(&$variables) {
  $node = $variables['node'];
  if ($variables['view_mode'] == 'full' && node_is_page($variables['node'])) {
    $variables['classes_array'][] = 'node-full';
  }
}

function sefovi_theme_form_user_login_block_alter(&$form, &$form_state, $form_id) {
  $form['#action'] = url(current_path(), array('query' => drupal_get_destination(), 'external' => FALSE));
  $form['#id'] = 'user-login-form';
  $form['#validate'] = user_login_default_validators();
  $form['#submit'][] = 'user_login_submit';
  $form['name'] = array('#type' => 'textfield',
    '#title' => t('Korisničko ime:'),
    '#maxlength' => USERNAME_MAX_LENGTH,
    '#size' => 15,
    '#required' => TRUE,
  );
  $form['pass'] = array('#type' => 'password',
    '#title' => t('Lozinka:'),
    '#size' => 15,
    '#required' => TRUE,
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit',
    '#value' => t('Ulogujte se'),
  );
  $items = array();
  if (variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL)) {
    $items[] = l(t('Kreirajte nalog'), 'user/register', array('attributes' => array('title' => t('Kreirajte novi korisnički nalog.'))));
  }
  $items[] = l(t('Nova lozinka'), 'user/password', array('attributes' => array('title' => t('Nova lozinka poslata na mail.'))));
  $form['links'] = array('#markup' => theme('item_list', array('items' => $items)));
  return $form;
}

function sefovi_theme_form_alter(&$form, &$form_state, $form_id){
  if($form_id === 'contact_site_form') {
    $form['name']['#title'] = t('Vaše ime');
    $form['mail']['#title'] = t('Vaša e-mail adresa');
	$form['subject']['#title'] = t('Naslov');
	$form['message']['#title'] = t('Poruka');
	$form['copy']['#title'] = t('Pošaljite kopiju i sebi.');
	$form['actions']['submit']['#value'] = t('Pošaljite poruku.');
  }
  
}




