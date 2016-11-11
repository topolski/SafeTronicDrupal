<?php

/**
 * @file
 * Korpa admin settings forms.
 */

/**
 * Callback for the admin configuration page.
 */
function korpa_admin_content_type() {
  module_load_include('inc', 'korpa', 'korpa.korpa');
  $node_types = node_type_get_types();
  if (empty($node_types)) {
    return NULL;
  }

  $options = array();
  foreach ($node_types as $node_type => $type) {
    if ($node_type == 'order') {
      continue;
    }
    $options[$node_type] = check_plain($type->name);
  }
  $default_value = array();
  
  foreach (korpa_product_types() as $product_type) {
    if (isset($options[$product_type])){
      $default_value[$product_type] = $product_type;
    }
  }

  $form['content_type'] = array(
    '#title' => t('Izbor content type-a'),
    '#type' => 'fieldset',
    '#description' => t('Izaberite content type za koji želite da se omogući "Dodaj u korpu" opcija.'),
  );

  $form['content_type']['korpa_content_types'] = array(
    '#title' => t('Content type-ovi'),
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => $default_value,
  );
  
  $form['currency'] = array(
    '#title' => t('Valuta'),
    '#type' => 'fieldset',
    '#description' => t('Izaberite valutu po kojoj želite da računate cenu proizvoda.'),
  );

  $form['currency']['korpa_currency'] = array(
    '#title' => t('Valuta'),
    '#type' => 'textfield',
    '#description' => t("Izaberite valutu."),
    '#default_value' => variable_get('korpa_currency'),
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Sačuvajte podešavanja korpe'));

  return $form;
}

/**
 * Callback submit function for the settings page.
 */
function korpa_admin_content_type_submit($form, &$form_state) {
    
  // Remove internal Form API values.
  unset($form_state['values']['form_id'], $form_state['values']['form_token'], $form_state['values']['form_build_id'], $form_state['values']['op']);

  $product_types = array();
  
  // Setting up the price field for the selected content types.
  $content_types = $form_state['values']['korpa_content_types'];
  
  // Unset to prevent 'double' save
  unset($form_state['values']['korpa_content_types']);
  
  if (!empty($content_types) && is_array($content_types)) {
    foreach (korpa_get_fields() as $field_name => $field_) {
      // Check to see if the field already exists.
      $field = field_info_field($field_name);
      // If the field does not exist then create it.
      if (empty($field)) {
        $field = array(
          'field_name' => $field_name,
          'type' => $field_['type'],
          'entity_types' => array('node'),
        );
        field_create_field($field);
      }
    }
    
    foreach ($content_types as $type => $checked) {
      
      // If a node type is checked, then create the price field.
      if ($checked) {
        // Save content_type as a product.
        $product_types[$type] = $type;
        
        foreach (korpa_get_fields() as $field_name => $field_) {
          // Foreach checked content type, we must assign the price field to the content type.
          $instance = field_info_instance('node', $field_name, $type);

          if (empty($instance)) {
            $instance = array(
              'field_name' => $field_name,
              'label' => $field_['title'],
              'description' => $field_['description'],
              'entity_type' => 'node',
              'bundle' => $type,
            );
            // It doesn't exist. Create it.
            field_create_instance($instance);
          }
        }
      }
      // If not, then delete the instance.
      else {
        foreach (korpa_get_fields() as $field_name => $field_) {
          $instance = field_info_instance('node', $field_name, $type);
          if (!empty($instance)) {
            field_delete_instance($instance);
          }
        }
      }
    }
  }

  variable_set('korpa_content_types', $product_types);
  variable_set('korpa_currency',$form_state['values']['korpa_currency']);
  
  drupal_set_message(t('Konfiguracija korpe je sačuvana.'));
}

