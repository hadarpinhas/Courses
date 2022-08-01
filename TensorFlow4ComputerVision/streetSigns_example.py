

from sklearn.model_selection import learning_curve
from my_utils import create_generators, order_test_set, split_data
from deepLearningModels import streetsigns_model
# from tensorflow.keras.callbacks import ModelCheckpoint, EarlyS  
from tensorflow.python.keras.callbacks import ModelCheckpoint
import tensorflow as tf


if __name__=='__main__':
    if False:
        path_to_data = 'C:\\Users\\User\\Downloads\\archive\\Train'
        path_to_save_train = 'C:\\Users\\User\\Downloads\\archive\\triaining_data\\train'
        path_to_save_val = 'C:\\Users\\User\\Downloads\\archive\\triaining_data\\val'

        split_data(path_to_data=path_to_data, path_to_save_train=path_to_save_train, path_to_save_val=path_to_save_val)
    
    if False:
        path_to_images = "C:\\Users\\User\\Downloads\\archive\\Test"
        path_to_csv = "C:\\Users\\User\\Downloads\\archive\\Test.csv"
        order_test_set(path_to_images, path_to_csv)        


    path_to_train = 'C:\\Users\\User\\Downloads\\archive\\triaining_data\\train'
    path_to_val = 'C:\\Users\\User\\Downloads\\archive\\triaining_data\\val'
    path_to_test = 'C:\\Users\\User\\Downloads\\archive\\Test'
    batch_size = 64
    epochs = 3
    lr=0.0001
    
    train_generator, val_generator, test_generator = create_generators(batch_size, path_to_train, path_to_val, path_to_test)
    nbr_classes = train_generator.num_classes

    TRAIN = False
    TEST = True
    if TRAIN:
        path_to_save_model = './Models'
        ckpt_saver = ModelCheckpoint(
            path_to_save_model,
            monitor='val_accuracy',
            mode='max',
            save_best_only=True,
            save_freq='epoch',
            verbose=1
        )

        early_stop = EarlyStopping(monitor='val_accuracy', patience=10

        )

        model = streetsigns_model(nbr_classes)

        optimizer = tf.keras.optimizers.Adam(learning_rate=lr, amsgrad=True)
        model.compile(optimizer=optimizer, loss='categorical_crossentropy', metrics=['accuracy'])

        model.fit(train_generator,
                    epochs=epochs,
                    batch_size=batch_size,
                    validation_data=val_generator,
                    callbacks=[ckpt_saver, early_stop]
        )

    if TEST:
        model = tf.keras.models.load_model('./Models')
        model.summary()

        print("evaluate validation set: ")
        model.evaluate(val_generator)

        print("evaluate test set: ")
        model.evaluate(test_generator)

